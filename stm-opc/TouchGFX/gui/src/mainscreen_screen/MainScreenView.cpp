#include <gui/mainscreen_screen/MainScreenView.hpp>

extern "C" 
{
#include "opcua.h"
#include "cmsis_os.h"
}

MainScreenView::MainScreenView()
{

}

void MainScreenView::setupScreen()
{
    MainScreenViewBase::setupScreen();
}

void MainScreenView::tearDownScreen()
{
    MainScreenViewBase::tearDownScreen();
}

void MainScreenView::getTime() {
	uint8_t flag = 1;
	osMessageQueuePut(opcuaRequestQueueHandle, &flag, 0, 0);
}

void MainScreenView::updateTime(Tempo tempo) {
	digitalClock.setTime24Hour(tempo.hour, tempo.min, tempo.sec);
	digitalClock.invalidate();
}

void MainScreenView::updateGauge(double availMemory) {
    gauge.updateValue(100 - availMemory, 30);
}
