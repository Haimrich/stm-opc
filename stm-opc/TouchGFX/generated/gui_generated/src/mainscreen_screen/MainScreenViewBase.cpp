/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <touchgfx/Color.hpp>
#include <BitmapDatabase.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>


MainScreenViewBase::MainScreenViewBase() :
    buttonCallback(this, &MainScreenViewBase::buttonCallbackHandler)
{

    touchgfx::CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

    __background.setPosition(0, 0, 800, 480);
    __background.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));

    backgroundBox.setPosition(0, 0, 800, 480);
    backgroundBox.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));

    dynamicGraph.setScale(10);
    dynamicGraph.setPosition(507, 310, 251, 129);
    dynamicGraph.setGraphAreaMargin(0, 0, 0, 0);
    dynamicGraph.setGraphAreaPadding(0, 0, 0, 0);
    dynamicGraph.setGraphRangeY(0, 100);

    dynamicGraphLine1Painter.setColor(touchgfx::Color::getColorFromRGB(20, 151, 197));
    dynamicGraphLine1.setPainter(dynamicGraphLine1Painter);
    dynamicGraphLine1.setLineWidth(3);
    dynamicGraph.addGraphElement(dynamicGraphLine1);

    dynamicGraph.addDataPoint(60.16293945111559f);
    dynamicGraph.addDataPoint(52.11563139340369f);
    dynamicGraph.addDataPoint(48.625070811254304f);
    dynamicGraph.addDataPoint(50.3035908222806f);
    dynamicGraph.addDataPoint(56.305298744751745f);
    dynamicGraph.addDataPoint(64.55051973211931f);
    dynamicGraph.addDataPoint(72.25930204120178f);
    dynamicGraph.addDataPoint(76.66239436804011f);
    dynamicGraph.addDataPoint(75.71441257537371f);
    dynamicGraph.addDataPoint(68.63318630687647f);
    dynamicGraph.addDataPoint(56.13169763400455f);
    dynamicGraph.addDataPoint(40.28421171392432f);
    dynamicGraph.addDataPoint(24.057754732708737f);
    dynamicGraph.addDataPoint(10.62208039572877f);
    dynamicGraph.addDataPoint(2.6056186595542314f);
    dynamicGraph.addDataPoint(1.4783002021373512f);
    dynamicGraph.addDataPoint(7.211311414411572f);
    dynamicGraph.addDataPoint(18.296301389294406f);
    dynamicGraph.addDataPoint(32.118853226462456f);
    dynamicGraph.addDataPoint(45.5946147720369f);
    dynamicGraph.addDataPoint(55.91249972365408f);
    dynamicGraph.addDataPoint(61.2034748501391f);
    dynamicGraph.addDataPoint(60.97196606820542f);
    dynamicGraph.addDataPoint(56.18530515520132f);
    dynamicGraph.addDataPoint(49.00058853064265f);
    dynamicGraph.addDataPoint(42.19726945687526f);
    dynamicGraph.addDataPoint(38.455971960846696f);
    dynamicGraph.addDataPoint(39.661718011245334f);
    dynamicGraph.addDataPoint(46.40377664654277f);
    dynamicGraph.addDataPoint(57.796129075668645f);
    dynamicGraph.addDataPoint(71.66391171181458f);
    dynamicGraph.addDataPoint(85.05140548031369f);
    dynamicGraph.addDataPoint(94.92817333466941f);
    dynamicGraph.addDataPoint(98.92115839339715f);
    dynamicGraph.addDataPoint(95.89389218076309f);
    dynamicGraph.addDataPoint(86.23107458947518f);
    dynamicGraph.addDataPoint(71.75859750288393f);
    dynamicGraph.addDataPoint(55.318027111006515f);
    dynamicGraph.addDataPoint(40.09886796940421f);
    dynamicGraph.addDataPoint(28.89097849087916f);
    dynamicGraph.addDataPoint(23.438844874090414f);
    dynamicGraph.addDataPoint(24.054320281351785f);
    dynamicGraph.addDataPoint(29.581057195472493f);
    dynamicGraph.addDataPoint(37.71771392999272f);
    dynamicGraph.addDataPoint(45.619205167684186f);
    dynamicGraph.addDataPoint(50.62728265041593f);
    dynamicGraph.addDataPoint(50.95021784304113f);
    dynamicGraph.addDataPoint(46.12402757547869f);
    dynamicGraph.addDataPoint(37.141418673447106f);
    dynamicGraph.addDataPoint(26.216270094422704f);
    dynamicGraph.addDataPoint(16.241015083565202f);
    dynamicGraph.addDataPoint(10.069798684473454f);
    dynamicGraph.addDataPoint(9.803264084877277f);
    dynamicGraph.addDataPoint(16.250731868997846f);
    dynamicGraph.addDataPoint(28.702386734351826f);
    dynamicGraph.addDataPoint(45.06843039391644f);
    dynamicGraph.addDataPoint(62.35251318604587f);
    dynamicGraph.addDataPoint(77.34505407934948f);
    dynamicGraph.addDataPoint(87.36830762865279f);
    dynamicGraph.addDataPoint(90.89238367764446f);
    dynamicGraph.addDataPoint(87.87297453825117f);
    dynamicGraph.addDataPoint(79.72957209442932f);
    dynamicGraph.addDataPoint(68.97080756283033f);
    dynamicGraph.addDataPoint(58.55972602194046f);
    dynamicGraph.addDataPoint(51.17521644744507f);
    dynamicGraph.addDataPoint(48.550947874445825f);
    dynamicGraph.addDataPoint(51.05386947917691f);
    dynamicGraph.addDataPoint(57.60535447076294f);
    dynamicGraph.addDataPoint(65.9638564948865f);
    dynamicGraph.addDataPoint(73.29913395678645f);
    dynamicGraph.addDataPoint(76.91643937056583f);
    dynamicGraph.addDataPoint(74.95212002453238f);
    dynamicGraph.addDataPoint(66.8688956397179f);
    dynamicGraph.addDataPoint(53.62799622872228f);
    dynamicGraph.addDataPoint(37.494396867445204f);
    dynamicGraph.addDataPoint(21.521217381045705f);
    dynamicGraph.addDataPoint(8.837970166929026f);
    dynamicGraph.addDataPoint(1.9154938562411097f);
    dynamicGraph.addDataPoint(1.9862997804737539f);
    dynamicGraph.addDataPoint(8.76124274207045f);
    dynamicGraph.addDataPoint(20.511150035216247f);
    dynamicGraph.addDataPoint(34.49299103322727f);
    dynamicGraph.addDataPoint(47.61613434436793f);
    dynamicGraph.addDataPoint(57.18578200283035f);
    dynamicGraph.addDataPoint(61.54209086904951f);
    dynamicGraph.addDataPoint(60.43932518021158f);
    dynamicGraph.addDataPoint(55.07329732165241f);
    dynamicGraph.addDataPoint(47.751681582752894f);
    dynamicGraph.addDataPoint(41.28939207735821f);
    dynamicGraph.addDataPoint(38.27863671834048f);
    dynamicGraph.addDataPoint(40.41398792117637f);
    dynamicGraph.addDataPoint(48.0393227623309f);
    dynamicGraph.addDataPoint(60.02907752982107f);
    dynamicGraph.addDataPoint(74.03426841705016f);
    dynamicGraph.addDataPoint(87.03422173937929f);
    dynamicGraph.addDataPoint(96.05986309100658f);
    dynamicGraph.addDataPoint(98.91212817844365f);
    dynamicGraph.addDataPoint(94.69994269479025f);
    dynamicGraph.addDataPoint(84.06607236478283f);
    dynamicGraph.addDataPoint(69.04524063895104f);

    gauge.setBackground(touchgfx::Bitmap(BITMAP_BLUE_GAUGES_ORIGINAL_GAUGE_BACKGROUND_STYLE_01_ID));
    gauge.setPosition(515, 25, 251, 251);
    gauge.setCenter(125, 125);
    gauge.setStartEndAngle(-85, 85);
    gauge.setRange(0, 100);
    gauge.setValue(50);
    gauge.setEasingEquation(touchgfx::EasingEquations::cubicEaseInOut);
    gauge.setNeedle(BITMAP_BLUE_NEEDLES_ORIGINAL_GAUGE_NEEDLE_STYLE_01_ID, 11, 55);
    gauge.setMovingNeedleRenderingAlgorithm(touchgfx::TextureMapper::NEAREST_NEIGHBOR);
    gauge.setSteadyNeedleRenderingAlgorithm(touchgfx::TextureMapper::BILINEAR_INTERPOLATION);
    gauge.setArcVisible();
    gaugePainter.setBitmap(touchgfx::Bitmap(BITMAP_BLUE_GAUGES_ORIGINAL_GAUGE_FILL_STYLE_01_ID));
    gauge.getArc().setPainter(gaugePainter);
    gauge.getArc().setRadius(94);
    gauge.getArc().setLineWidth(14);
    gauge.getArc().setCapPrecision(180);
    gauge.setArcPosition(28, 30, 196, 88);

    getTimeButton.setXY(151, 180);
    getTimeButton.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID));
    getTimeButton.setAction(buttonCallback);

    digitalClock.setPosition(49, 59, 375, 104);
    digitalClock.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    digitalClock.setTypedText(touchgfx::TypedText(T___SINGLEUSE_BM3R));
    digitalClock.displayLeadingZeroForHourIndicator(true);
    digitalClock.setDisplayMode(touchgfx::DigitalClock::DISPLAY_24_HOUR);
    digitalClock.setTime24Hour(0, 0, 0);

    yAxisLine.setPosition(500, 303, 30, 163);
    yAxisLinePainter.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    yAxisLine.setPainter(yAxisLinePainter);
    yAxisLine.setStart(8, 6);
    yAxisLine.setEnd(7, 138);
    yAxisLine.setLineWidth(3);
    yAxisLine.setLineEndingStyle(touchgfx::Line::ROUND_CAP_ENDING);

    xAxisLine.setPosition(496, 432, 266, 34);
    xAxisLinePainter.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    xAxisLine.setPainter(xAxisLinePainter);
    xAxisLine.setStart(8, 6);
    xAxisLine.setEnd(400, 6);
    xAxisLine.setLineWidth(3);
    xAxisLine.setLineEndingStyle(touchgfx::Line::ROUND_CAP_ENDING);

    valueText.setXY(739, 291);
    valueText.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    valueText.setLinespacing(0);
    valueText.setTypedText(touchgfx::TypedText(T___SINGLEUSE_4PND));

    add(__background);
    add(backgroundBox);
    add(dynamicGraph);
    add(gauge);
    add(getTimeButton);
    add(digitalClock);
    add(yAxisLine);
    add(xAxisLine);
    add(valueText);
}

void MainScreenViewBase::setupScreen()
{

}

void MainScreenViewBase::buttonCallbackHandler(const touchgfx::AbstractButton& src)
{
    if (&src == &getTimeButton)
    {
        //getTimeButton
        //When getTimeButton clicked call virtual function
        //Call getTime
        getTime();
    }
}
