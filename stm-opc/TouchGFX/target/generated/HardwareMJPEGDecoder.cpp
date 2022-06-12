
#include <HardwareMJPEGDecoder.hpp>

extern "C"
{
  #include <main.h>
  #include <string.h>
  #include <jpeg_utils.h>
  #include <stm32f7xx_hal.h>

  uint32_t JPEG_Decode_DMA(JPEG_HandleTypeDef *hjpeg, uint8_t* input, uint32_t chunkSizeIn, uint8_t* output);
  uint32_t JPEG_OutputHandler(JPEG_HandleTypeDef *hjpeg);
  void HAL_JPEG_DecodeCpltCallback(JPEG_HandleTypeDef *hjpeg);
  void HAL_JPEG_ErrorCallback(JPEG_HandleTypeDef *hjpeg);

  HAL_StatusTypeDef TGFX_JPEG_GetDecodeColorConvertFunc(JPEG_ConfTypeDef *pJpegInfo, JPEG_YCbCrToRGB_Convert_Function *pFunction, uint32_t *ImageNbMCUs);
  void TGFX_JPEG_InitColorTables(void);
  namespace
  {
    uint8_t* FrameBufferAddress;
    uint32_t JPEG_InputImageIndex;
    uint32_t JPEG_InputImageSize_Bytes;
    uint32_t JPEG_InputImageAddress;
    uint32_t Jpeg_HWDecodingEnd = 0;
    uint32_t Jpeg_GoDecodeNextFrame = 0;
    uint32_t JPEG_IN_Read_BufferIndex = 0;
    uint32_t JPEG_IN_Write_BufferIndex = 0;
    volatile uint32_t Input_Is_Paused = 0;
    uint32_t JPEG_OUT_Read_BufferIndex = 0;
    uint32_t JPEG_OUT_Write_BufferIndex = 0;
    volatile uint32_t Output_Is_Paused = 0;
    volatile uint32_t JpegProcessing_End = 0;
    uint32_t MCU_TotalNb = 0;
    JPEG_YCbCrToRGB_Convert_Function pConvert_Function;
  }
}

#define MCU_WIDTH_PIXELS            ((uint32_t)16)
#define MCU_HEIGHT_PIXELS           ((uint32_t)16)
#define MCU_CHROMA_420_SIZE_BYTES   ((uint32_t)384)

#define CHUNK_SIZE_IN  ((uint32_t)(1024*64))  //Max block size
#define CHUNK_SIZE_OUT ((uint32_t)(MCU_CHROMA_420_SIZE_BYTES * (800 / MCU_WIDTH_PIXELS)))

#define JPEG_BUFFER_EMPTY 0
#define JPEG_BUFFER_FULL  1
#define NB_OUTPUT_DATA_BUFFERS      2

typedef struct
{
  uint8_t State;
  uint8_t *DataBuffer;
  uint32_t DataBufferSize;
} JPEG_Data_BufferTypeDef;

#ifdef __GNUC__
uint8_t MCU_Data_OutBuffer0[CHUNK_SIZE_OUT] __attribute__ ((section ("CCMRAM")));
uint8_t MCU_Data_OutBuffer1[CHUNK_SIZE_OUT] __attribute__ ((section ("CCMRAM")));
#else
#pragma location="CCMRAM"
uint8_t MCU_Data_OutBuffer0[CHUNK_SIZE_OUT];
uint8_t MCU_Data_OutBuffer1[CHUNK_SIZE_OUT];
#endif

// Zero initialize YCbCr buffers
JPEG_Data_BufferTypeDef Jpeg_OUT_BufferTab[NB_OUTPUT_DATA_BUFFERS] =
{
  {JPEG_BUFFER_EMPTY , MCU_Data_OutBuffer0 , 0},
  {JPEG_BUFFER_EMPTY , MCU_Data_OutBuffer1 , 0},
};

extern JPEG_ConfTypeDef*    JPEG_Info;
extern JPEG_HandleTypeDef  hjpeg;

__IO uint32_t MCU_BlockIndex = 0;

SEM_TYPE semDecodingDone;

static struct JPEG_MCU_RGB_Converter {
  uint32_t WidthExtend;
  uint32_t ScaledWidth;
  uint32_t LastLineHeight;
  uint32_t MCU_pr_line;
  uint32_t bytes_pr_pixel;
} JPEG_ConvertorParams;

HardwareMJPEGDecoder::HardwareMJPEGDecoder()
    : frameNumber(0), currentMovieOffset(0), indexOffset(0), firstFrameOffset(0), lastFrameEnd(0), movieLength(0), movieData(0),
      reader(0), aviBuffer(0), aviBufferLength(0), aviBufferStartOffset(0), lastError(AVI_NO_ERROR)
{
    //Clear video info
    videoInfo.frame_height = 0;
    videoInfo.frame_width = 0;
    videoInfo.ms_between_frames = 0;
    videoInfo.number_of_frames = 0;

    //Init color tables
    TGFX_JPEG_InitColorTables();

    //Create decoding semaphore
    semDecodingDone = SEM_CREATE();
}

int HardwareMJPEGDecoder::compare(const uint32_t offset, const char* str, uint32_t num)
{
    const char* src;
    if (reader != 0)
    {
        // Assuming data is in buffer!
        src = reinterpret_cast<const char*>(aviBuffer + (offset - aviBufferStartOffset));
    }
    else
    {
        src = (const char*)movieData + offset;
    }
    return strncmp(src, str, num);
}

inline uint32_t HardwareMJPEGDecoder::getU32(const uint32_t offset)
{
    if (reader != 0)
    {
        // Assuming data is in buffer!
        const uint32_t index = offset - aviBufferStartOffset;
        return aviBuffer[index+0] | (aviBuffer[index+1]<<8) | (aviBuffer[index+2]<<16) | (aviBuffer[index+3]<<24);
    }
    else
    {
        const uint8_t* const d = movieData + offset;
        return d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
    }
}

inline uint32_t HardwareMJPEGDecoder::getU16(const uint32_t offset)
{
    if (reader != 0)
    {
        // Assuming data is in buffer!
        const uint32_t index = offset - aviBufferStartOffset;
        return aviBuffer[index+0] | (aviBuffer[index+1]<<8);
    }
    else
    {
        const uint8_t* const d = movieData + offset;
        return d[0] | (d[1] << 8);
    }
}

const uint8_t* HardwareMJPEGDecoder::readData(uint32_t offset, uint32_t length)
{
    if (reader != 0)
    {
        if (length > aviBufferLength)
        {
            lastError = AVI_ERROR_FILE_BUFFER_TO_SMALL;
            assert(!"Buffer to small");
        }

        reader->seek(offset);
        if (!reader->readData(aviBuffer, length))
        {
            lastError = AVI_ERROR_EOF_REACHED;
        }

        aviBufferStartOffset = offset;
        return aviBuffer;
    }

    return movieData + offset;
}

bool HardwareMJPEGDecoder::decodeNextFrame(uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height, uint32_t buffer_stride)
{
    assert((frameNumber > 0) && "HardwareMJPEGDecoder decoding without frame data!");

    //find next frame and decode it
    readData(currentMovieOffset, 8);
    uint32_t streamNo  = getU16(currentMovieOffset);
    uint32_t chunkType = getU16(currentMovieOffset + 2);
    uint32_t chunkSize = getU32(currentMovieOffset + 4);

    const uint16_t STREAM0 = 0x3030;
    const uint16_t TYPEDC  = 0x6364;

    bool isCurrentFrameLast;
    //play frame if we have it all
    if (currentMovieOffset + 8 + chunkSize < movieLength)
    {
        if (streamNo == STREAM0 && chunkType == TYPEDC && chunkSize > 0)
        {
            currentMovieOffset += 8;
            //decode frame
            const uint8_t* chunk = readData(currentMovieOffset, chunkSize);
            decodeMJPEGFrame(chunk, chunkSize, buffer, buffer_width, buffer_height, buffer_stride);
            frameNumber++;
        }

        isCurrentFrameLast = false;

        // Advance to next frame
        currentMovieOffset += chunkSize;
        if (chunkSize == 0) // Empty frame - Skip
        {
          currentMovieOffset += 8;
        }
        currentMovieOffset = (currentMovieOffset + 1) & 0xFFFFFFFE; //pad to next word

        if (currentMovieOffset == lastFrameEnd)
        {
            frameNumber = 1;
            currentMovieOffset = firstFrameOffset; //start over
            isCurrentFrameLast = true;
        }
    }
    else
    {
        frameNumber = 1;
        currentMovieOffset = firstFrameOffset; //start over
        isCurrentFrameLast = true;
    }
    return !isCurrentFrameLast;
}

bool HardwareMJPEGDecoder::gotoNextFrame()
{
    assert((frameNumber > 0) && "HardwareMJPEGDecoder decoding without frame data!");

    readData(currentMovieOffset, 8);
    uint32_t chunkSize = getU32(currentMovieOffset + 4);

    //increment until next video frame
    while (currentMovieOffset + 8 + chunkSize < movieLength)
    {
        //increment one frame
        currentMovieOffset += chunkSize + 8;
        currentMovieOffset = (currentMovieOffset + 1) & 0xFFFFFFFE; //pad to next word
        frameNumber++;

        //next chunk
        readData(currentMovieOffset, 8);
        //check it is a video frame
        uint32_t streamNo  = getU16(currentMovieOffset);
        uint32_t chunkType = getU16(currentMovieOffset + 2);
        chunkSize = getU32(currentMovieOffset + 4);
        const uint16_t STREAM0 = 0x3030;
        const uint16_t TYPEDC  = 0x6364;

        if (streamNo == STREAM0 && chunkType == TYPEDC)
        {
            // Found next frame
            return true;
        }
    }

    //skip back to first frame
    frameNumber = 1;
    currentMovieOffset = firstFrameOffset; //start over
    return false;
}

void HardwareMJPEGDecoder::setVideoData(const uint8_t* movie, const uint32_t length)
{
    movieData = movie;
    movieLength = length;
    reader = 0; //not using reader

    readVideoHeader();
}

void HardwareMJPEGDecoder::setVideoData(touchgfx::VideoDataReader& reader)
{
    this->reader = &reader;
    movieData = 0;
    movieLength = reader.getDataLength();

    readVideoHeader();
}

bool HardwareMJPEGDecoder::hasVideo()
{
    return (reader != 0) || (movieData != 0);
}

void HardwareMJPEGDecoder::readVideoHeader()
{
    // Start from the start
    currentMovieOffset = 0;
    lastError = AVI_NO_ERROR;

    // Make header available in buffer
    readData(0, 72);

    // Decode the movie header to find first frame
    // Must be RIFF file
    if (compare(currentMovieOffset, "RIFF", 4))
    {
        lastError = AVI_ERROR_NOT_RIFF;
        assert(!"RIFF header not found");
    }

    //skip fourcc and length
    currentMovieOffset += 8;
    if (compare(currentMovieOffset, "AVI ", 4))
    {
        lastError = AVI_ERROR_AVI_HEADER_NOT_FOUND;
        assert(!"AVI header not found");
    }

    currentMovieOffset += 4;
    if (compare(currentMovieOffset, "LIST", 4))
    {
        lastError = AVI_ERROR_AVI_LIST_NOT_FOUND;
        assert(!"AVI LIST not found");
    }

    //save AVI List info
    const uint32_t aviListSize = getU32(currentMovieOffset + 4);
    const uint32_t aviListOffset = currentMovieOffset;
    assert(aviListSize);

    //look into header to find frame rate
    bool foundFrame = true;
    uint32_t offset =  currentMovieOffset + 8;
    if (compare(offset, "hdrl", 4))
    {
        lastError = AVI_ERROR_AVI_HDRL_NOT_FOUND;
        foundFrame = false;
    }

    offset += 4;
    if (compare(offset, "avih", 4))
    {
        lastError = AVI_ERROR_AVI_AVIH_NOT_FOUND;
        foundFrame = false;
    }

    if (foundFrame)
    {
        offset += 8; //skip fourcc and cb in AVIMAINHEADER
        videoInfo.ms_between_frames = getU32(offset)/1000;
        videoInfo.number_of_frames = getU32(offset + 16);
        videoInfo.frame_width = getU32(offset + 32);
        videoInfo.frame_height = getU32(offset + 36);
    }
    //skip rest of AVI header, start from end of AVI List

    //look for list with 'movi' header
    uint32_t listOffset = aviListOffset + aviListSize + 8;
    readData(listOffset, 12);
    while (compare(listOffset + 8, "movi", 4) && (lastError == AVI_NO_ERROR) && listOffset < movieLength)
    {
        const uint32_t listSize = getU32(listOffset + 4) + 8;
        listOffset += listSize;
        readData(listOffset, 12);
    }

    if (lastError != AVI_NO_ERROR)
    {
        lastError = AVI_ERROR_MOVI_NOT_FOUND;
        return;
    }

    //save first frame and end of last frame
    currentMovieOffset = listOffset + 8 + 4; //skip LIST and 'movi'
    lastFrameEnd = listOffset + 8 + getU32(listOffset + 4);

    //find idx
    const uint32_t listSize = getU32(listOffset+4)+8;
    listOffset += listSize;
    readData(listOffset, 4);
    if (!compare(listOffset, "idx1", 4))
    {
        indexOffset = listOffset;
    }
    else
    {
        lastError = AVI_ERROR_IDX1_NOT_FOUND;
        return;
    }

    //start on first frame
    frameNumber = 1; //next frame number is 1
    firstFrameOffset = currentMovieOffset;
}

void HardwareMJPEGDecoder::decodeMJPEGFrame(const uint8_t* const mjpgdata, const uint32_t length, uint8_t* outputBuffer, uint16_t bufferWidth, uint16_t bufferHeight, uint32_t bufferStride)
{
    if (length == 0)
    {
        return;
    }

    if (outputBuffer) //only decode if buffers are assigned.
    {
      // Update JPEG conversion parameters
      JPEG_ConvertorParams.bytes_pr_pixel = 2;
      JPEG_ConvertorParams.WidthExtend = videoInfo.frame_width;
      if((JPEG_ConvertorParams.WidthExtend % 16) != 0)
      {
          JPEG_ConvertorParams.WidthExtend += 16 - (JPEG_ConvertorParams.WidthExtend % 16);
      }
      JPEG_ConvertorParams.ScaledWidth = 800 * JPEG_ConvertorParams.bytes_pr_pixel;
      JPEG_ConvertorParams.MCU_pr_line = JPEG_ConvertorParams.WidthExtend / MCU_WIDTH_PIXELS;
      JPEG_ConvertorParams.LastLineHeight = 16 - (videoInfo.frame_height % MCU_HEIGHT_PIXELS);

      JPEG_Decode_DMA(&hjpeg, const_cast<uint8_t*>(mjpgdata), length, outputBuffer);
      do
      {
        JpegProcessing_End = JPEG_OutputHandler(&hjpeg);

        //If nothing to do, allow other tasks
        if (JpegProcessing_End == 2)
        {
          SEM_WAIT(semDecodingDone);
        }
      }
      while(JpegProcessing_End != 1);
    }
}

bool HardwareMJPEGDecoder::decodeFrame(const touchgfx::Rect& area, uint8_t* frameBuffer, uint32_t framebuffer_width)
{
     // Assuming that chunk is available and streamNo and chunkType is correct.
    // Check by gotoNextFrame

    readData(currentMovieOffset, 8);
    const uint32_t length = getU32(currentMovieOffset + 4);

    // Ensure whole frame is read
    const uint8_t* mjpgdata = readData(currentMovieOffset + 8, length);

    if (frameBuffer) //only decode if buffers are assigned.
    {
      JPEG_Decode_DMA(&hjpeg, const_cast<uint8_t*>(mjpgdata), length, frameBuffer);

      do
      {
        JpegProcessing_End = JPEG_OutputHandler(&hjpeg);
      }
      while(JpegProcessing_End == 0);

    }
    return true;
}

bool HardwareMJPEGDecoder::decodeThumbnail(uint32_t frameno, uint8_t* buffer, uint16_t width, uint16_t height)
{
    assert(0);
    return false;
}

void HardwareMJPEGDecoder::gotoFrame(uint32_t frameNumber)
{
    if (frameNumber==0)
    {
        frameNumber = 1;
    }

    if (frameNumber > getNumberOfFrames())
    {
        frameNumber = getNumberOfFrames();
    }

    uint32_t offset = indexOffset + 8 + (frameNumber-1)*16;

    readData(offset, 16);

    currentMovieOffset = getU32(offset + 8) + firstFrameOffset - 4;
    this->frameNumber = frameNumber;
}

uint32_t HardwareMJPEGDecoder::getNumberOfFrames()
{
    return videoInfo.number_of_frames;
}

void HardwareMJPEGDecoder::setRepeatVideo(bool repeat)
{

}

void HardwareMJPEGDecoder::getVideoInfo(touchgfx::VideoInformation* data)
{
    *data = videoInfo;
}

/*   C HELPER FUNCTIONS */

extern "C"
{
/**
  * @brief  Decode_DMA
  * @param hjpeg: JPEG handle pointer
  * @param  JPEGImageBufferAddress : jpg image buffer Address.
  * @param  JPEGImageSize_Bytes    : jpg image size in bytes.
  * @param  DestAddress : ARGB8888 destination Frame Buffer Address.
  * @retval None
  */
uint32_t JPEG_Decode_DMA(JPEG_HandleTypeDef *hjpeg, uint8_t* input, uint32_t chunkSizeIn /* length */, uint8_t* output)
{
    FrameBufferAddress = output;
    Output_Is_Paused = 0;
    JPEG_OUT_Read_BufferIndex = 0;
    JPEG_OUT_Write_BufferIndex = 0;
    JPEG_InputImageIndex = 0;
    JPEG_InputImageAddress = (uint32_t)input;
    JPEG_InputImageSize_Bytes = chunkSizeIn;
    MCU_BlockIndex = 0;

    //Init buffers
    for(uint32_t i = 0; i < NB_OUTPUT_DATA_BUFFERS; ++i)
    {
      Jpeg_OUT_BufferTab[i].State = JPEG_BUFFER_EMPTY;
      Jpeg_OUT_BufferTab[i].DataBufferSize = 0;
    }

    //Do not return from this function until done with decoding all chunks.
    HAL_JPEG_Decode_DMA(hjpeg, input, CHUNK_SIZE_IN, Jpeg_OUT_BufferTab[JPEG_OUT_Write_BufferIndex].DataBuffer, MCU_CHROMA_420_SIZE_BYTES * JPEG_ConvertorParams.MCU_pr_line);

    return 0;
}

/**
  * @brief  JPEG Info ready callback
  * @param hjpeg: JPEG handle pointer
  * @param pInfo: JPEG Info Struct pointer
  * @retval None
  */
void HAL_JPEG_InfoReadyCallback(JPEG_HandleTypeDef *hjpeg, JPEG_ConfTypeDef *pInfo)
{
  if(pInfo->ChromaSubsampling == JPEG_420_SUBSAMPLING)
  {
    if((pInfo->ImageWidth % 16) != 0)
      pInfo->ImageWidth += (16 - (pInfo->ImageWidth % 16));

    if((pInfo->ImageHeight % 16) != 0)
      pInfo->ImageHeight += (16 - (pInfo->ImageHeight % 16));

    HAL_StatusTypeDef status = TGFX_JPEG_GetDecodeColorConvertFunc(pInfo, &pConvert_Function, &MCU_TotalNb);
    if(status != HAL_OK)
    {
      assert(status == HAL_OK);
    }
  }
  else
  {
    if(pInfo->ChromaSubsampling == JPEG_422_SUBSAMPLING)
    {
      if((pInfo->ImageWidth % 16) != 0)
        pInfo->ImageWidth += (16 - (pInfo->ImageWidth % 16));

      if((pInfo->ImageHeight % 8) != 0)
        pInfo->ImageHeight += (8 - (pInfo->ImageHeight % 8));
    }

    if(pInfo->ChromaSubsampling == JPEG_444_SUBSAMPLING)
    {
      if((pInfo->ImageWidth % 8) != 0)
        pInfo->ImageWidth += (8 - (pInfo->ImageWidth % 8));

      if((pInfo->ImageHeight % 8) != 0)
        pInfo->ImageHeight += (8 - (pInfo->ImageHeight % 8));
    }

    HAL_StatusTypeDef status = JPEG_GetDecodeColorConvertFunc(pInfo, &pConvert_Function, &MCU_TotalNb);
    if(status != HAL_OK)
    {
      assert(status == HAL_OK);
    }
  }
}

/**
  * @brief  JPEG Get Data callback.
  * @param hjpeg: JPEG handle pointer
  * @param NbDecodedData: Number of decoded (consummed) bytes from input buffer
  * @retval None
  */
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbDecodedData)
{
  //Input buffer has been consumed by the peripheral and to ask for a new data chunk if the operation (encoding/decoding) has not been complete yet.
  JPEG_InputImageIndex += NbDecodedData;
  if(JPEG_InputImageIndex < JPEG_InputImageSize_Bytes)
  {
    JPEG_InputImageAddress = JPEG_InputImageAddress + NbDecodedData;
    uint32_t inDataLength = JPEG_InputImageSize_Bytes - JPEG_InputImageIndex;
    HAL_JPEG_ConfigInputBuffer(hjpeg,(uint8_t *)JPEG_InputImageAddress, inDataLength >= CHUNK_SIZE_IN ? CHUNK_SIZE_IN : inDataLength);
  }
}

/**
  * @brief  JPEG Data Ready callback. Data has been converted from JPEG to YCbCr.
  * @param hjpeg: JPEG handle pointer
  * @param pDataOut: pointer to the output data buffer
  * @param OutDataLength: length of output buffer in bytes
  * @retval None
  */
void HAL_JPEG_DataReadyCallback (JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength)
{
    Jpeg_OUT_BufferTab[JPEG_OUT_Write_BufferIndex].State = JPEG_BUFFER_FULL;
    Jpeg_OUT_BufferTab[JPEG_OUT_Write_BufferIndex].DataBufferSize = OutDataLength;

    JPEG_OUT_Write_BufferIndex++;
    if(JPEG_OUT_Write_BufferIndex >= NB_OUTPUT_DATA_BUFFERS)
    {
      JPEG_OUT_Write_BufferIndex = 0;
    }

    //if the other buffer is full, then ui thread might be converting it
    if(Jpeg_OUT_BufferTab[JPEG_OUT_Write_BufferIndex].State != JPEG_BUFFER_EMPTY)
    {
      HAL_JPEG_Pause(hjpeg, JPEG_PAUSE_RESUME_OUTPUT);
      Output_Is_Paused = 1;
    }
    HAL_JPEG_ConfigOutputBuffer(hjpeg, Jpeg_OUT_BufferTab[JPEG_OUT_Write_BufferIndex].DataBuffer, MCU_CHROMA_420_SIZE_BYTES * JPEG_ConvertorParams.MCU_pr_line);

    // Signal Hardware Decoding to wake up
    SEM_POST(semDecodingDone);
}

/**
  * @brief  JPEG Error callback
  * @param hjpeg: JPEG handle pointer
  * @retval None
  */
void HAL_JPEG_ErrorCallback(JPEG_HandleTypeDef *hjpeg)
{
  HAL_JPEG_STATETypeDef  state = HAL_JPEG_GetState(hjpeg);
  uint32_t error = HAL_JPEG_GetError(hjpeg);
  while(1){}
}

/**
  * @brief  JPEG Decode complete callback
  * @param hjpeg: JPEG handle pointer
  * @retval None
  */
void HAL_JPEG_DecodeCpltCallback(JPEG_HandleTypeDef *hjpeg)
{
  Jpeg_HWDecodingEnd = 1;
}
}

/**
  * @brief  JPEG Ouput Data BackGround Postprocessing .
  * @param hjpeg: JPEG handle pointer
  * @retval 1 : if JPEG processing has finished, 0 : if JPEG processing still ongoing
  */
uint32_t JPEG_OutputHandler(JPEG_HandleTypeDef *hjpeg)
{
  uint32_t ConvertedDataCount;

  if(Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].State == JPEG_BUFFER_FULL)
  {
    /* Invalidate DCache prior to YCbCr/RGB software conversion. */
    if (SCB->CCR & SCB_CCR_DC_Msk)
    {
      SCB_CleanInvalidateDCache();
    }

    MCU_BlockIndex += pConvert_Function(Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].DataBuffer, FrameBufferAddress, MCU_BlockIndex, Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].DataBufferSize, &ConvertedDataCount);

    Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].State = JPEG_BUFFER_EMPTY;
    Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].DataBufferSize = 0;

    JPEG_OUT_Read_BufferIndex++;
    if(JPEG_OUT_Read_BufferIndex >= NB_OUTPUT_DATA_BUFFERS)
    {
      JPEG_OUT_Read_BufferIndex = 0;
    }

    if(MCU_BlockIndex == MCU_TotalNb)
    {
      return 1;
    }
  }
  else if((Output_Is_Paused == 1) && \
          (Jpeg_OUT_BufferTab[JPEG_OUT_Write_BufferIndex].State == JPEG_BUFFER_EMPTY) &&\
          (Jpeg_OUT_BufferTab[JPEG_OUT_Read_BufferIndex].State == JPEG_BUFFER_EMPTY))
  {
    Output_Is_Paused = 0;
    HAL_JPEG_Resume(hjpeg, JPEG_PAUSE_RESUME_OUTPUT);
  }
  else
  {
    return 2;
  }

  if((MCU_BlockIndex == MCU_TotalNb) && (Jpeg_HWDecodingEnd))// decode completed
  {
    return 1;
  }

  return 0;
}

