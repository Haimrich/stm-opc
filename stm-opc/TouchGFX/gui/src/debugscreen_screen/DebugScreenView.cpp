#include <gui/debugscreen_screen/DebugScreenView.hpp>

extern "C"
{
#include "opcua.h"
#include "cmsis_os.h"
}

DebugScreenView::DebugScreenView()
{

}

void DebugScreenView::setupScreen()
{
    DebugScreenViewBase::setupScreen();
}

void DebugScreenView::tearDownScreen()
{
    DebugScreenViewBase::tearDownScreen();
}

void DebugScreenView::getTime() {
	uint8_t flag = OPCUA_REQUEST_TIME;
	osMessageQueuePut(opcuaRequestQueueHandle, &flag, 0, 0);
}

void DebugScreenView::updateTime(Tempo tempo) {
	digitalClock.setTime24Hour(tempo.hour, tempo.min, tempo.sec);
	digitalClock.invalidate();
}

void DebugScreenView::updateGauge(double availMemory) {
    gauge.updateValue(100 - availMemory, 30);
}
