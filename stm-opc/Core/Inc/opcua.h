#ifndef OPCUA_H_
#define OPCUA_H_

#include "stdint.h"
#include "cmsis_os.h"

typedef struct Tempo {
	uint16_t hour;
	uint16_t min;
	uint16_t sec;
} Tempo;


extern osMessageQueueId_t opcuaRequestQueueHandle;
extern osMessageQueueId_t opcuaResultQueueHandle;

extern osMutexId_t opcuaMutexHandle;

extern double gOpcuaAvailableMemory;
extern Tempo gOpcuaTime;

#define OPCUA_NEW_AVAILABLE_MEMORY_VALUE 	0
#define OPCUA_NEW_TIME_VALUE 				1

#endif
