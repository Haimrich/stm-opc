#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <BitmapDatabase.hpp>

extern "C" 
{
#include "opcua.h"
#include "cmsis_os.h"
}

MainScreenView::MainScreenView() :
	valveClickedCallback(this, &MainScreenView::valveClickHandler),
	pumpClickedCallback(this, &MainScreenView::pumpClickHandler),
	fireClickedCallback(this, &MainScreenView::fireClickHandler)
{
	tickCounter = 0;
}

void MainScreenView::setupScreen()
{
    MainScreenViewBase::setupScreen();

    valve.setClickAction(valveClickedCallback);
    pump.setClickAction(pumpClickedCallback);
    fire.setClickAction(fireClickedCallback);
}

void MainScreenView::tearDownScreen()
{
    MainScreenViewBase::tearDownScreen();
}

void MainScreenView::valveClickHandler(const Image& img, const ClickEvent& e)
{
	if (e.getType() != ClickEvent::PRESSED) return;
	osMutexAcquire(opcuaMutexHandle, osWaitForever);
	gData.valveStatus = !gData.valveStatus;
	osMutexRelease(opcuaMutexHandle);

	uint8_t flag = OPCUA_REQUEST_SWITCH_VALVE;
	osMessageQueuePut(opcuaRequestQueueHandle, &flag, 0, 0);
}

void MainScreenView::pumpClickHandler(const Image& img, const ClickEvent& e)
{
	if (e.getType() != ClickEvent::PRESSED) return;
	osMutexAcquire(opcuaMutexHandle, osWaitForever);
	gData.pumpStatus = !gData.pumpStatus;
	osMutexRelease(opcuaMutexHandle);

	uint8_t flag = OPCUA_REQUEST_SWITCH_PUMP;
	osMessageQueuePut(opcuaRequestQueueHandle, &flag, 0, 0);
}

void MainScreenView::fireClickHandler(const Image& img, const ClickEvent& e)
{
	if (e.getType() != ClickEvent::PRESSED) return;
	osMutexAcquire(opcuaMutexHandle, osWaitForever);
	gData.fireStatus = !gData.fireStatus;
	osMutexRelease(opcuaMutexHandle);

	uint8_t flag = OPCUA_REQUEST_SWITCH_FIRE;
	osMessageQueuePut(opcuaRequestQueueHandle, &flag, 0, 0);
}

void MainScreenView::updateMainTankTemperature(double temp)
{
	Unicode::snprintfFloat(mainTankTempTextBuffer, 10, "%.1f", temp);
	mainTankTempText.invalidate();
}

void MainScreenView::updateMainTankLevel(double level)
{
	mainTank.setValue((int)(level*1000));
	mainTank.invalidate();
}

void MainScreenView::updateBoilerTemperature(double temp)
{
	Unicode::snprintfFloat(boilerTempTextBuffer, 10, "%.1f", temp);
	boilerTempText.invalidate();
}

void MainScreenView::updateBoilerLevel(double level)
{
	boiler.setValue((int)(level*1000));
	boiler.invalidate();
}

void MainScreenView::updateBoilerPressure(double pressure)
{
	pressureGauge.updateValue(pressure*1000, 50);
	Unicode::snprintfFloat(pressureTextBuffer, 10, "%.1f", pressure);
	pressureText.invalidate();
}

void MainScreenView::updateTurbinePower(double power)
{
	Unicode::snprintfFloat(energyTextBuffer, 10, "%.1f", power);
	energyText.invalidate();
}

void MainScreenView::updateValveStatus(bool status)
{
	valve.setBitmap(Bitmap(status ? BITMAP_VALVE_ON_ID : BITMAP_VALVE_ID));
	valve.invalidate();

	valveStatus = status;
}

void MainScreenView::updatePumpStatus(bool status)
{
	pump.setBitmap(Bitmap(status ? BITMAP_PUMP_ON_ID : BITMAP_PUMP_ID));
	pump.invalidate();

	pumpStatus = status;
}

void MainScreenView::updateFireStatus(bool status)
{
	fire.setBitmap(Bitmap(status ? BITMAP_FIRE_ON_ID : BITMAP_FIRE_ID));
	fire.invalidate();

	fireStatus = status;
}


void MainScreenView::handleTickEvent()
{
	// Update graphs
    tickCounter++;

    if (tickCounter % 60 == 0)
    {
      float mainTankLevel = (float)mainTank.getValue()/1000;
      mainTankGraph.addDataPoint(mainTankLevel);

      float boilerLevel = (float)boiler.getValue()/1000;
      boilerGraph.addDataPoint(boilerLevel);

      BitmapId targetId;

      targetId = valveStatus ? BITMAP_PIPE1_ON_ID : BITMAP_PIPE1_ID;
      if (pipe1.getBitmapId() != targetId) {
		  pipe1.setBitmap(Bitmap(targetId));
		  pipe1.invalidate();
      }

      targetId = mainTankLevel > 0 ? BITMAP_PIPE1_ON_ID : BITMAP_PIPE1_ID;
	  if (pipe3.getBitmapId() != targetId) {
	      pipe3.setBitmap(Bitmap(targetId));
	      pipe3.invalidate();
	  }

      targetId = (mainTankLevel > 0 && pumpStatus) ? BITMAP_PIPE1_ON_ID : BITMAP_PIPE1_ID;
	  if (pipe4.getBitmapId() != targetId) {
	      pipe4.setBitmap(Bitmap(targetId));
	      pipe4.invalidate();
	  }

      bool steamActive = boilerLevel > 0 && fireStatus;

      targetId = steamActive ? BITMAP_PIPE2_ON_ID : BITMAP_PIPE2_ID;
	  if (pipe5.getBitmapId() != targetId) {
	      pipe5.setBitmap(Bitmap(targetId));
	      pipe5.invalidate();
	  }

	  targetId = steamActive ? BITMAP_PIPE3_ON_ID : BITMAP_PIPE3_ID;
	  if (pipe6.getBitmapId() != targetId) {
		  pipe6.setBitmap(Bitmap(targetId));
		  pipe6.invalidate();
	  }

	  targetId = steamActive ? BITMAP_PIPE4_ON_ID : BITMAP_PIPE4_ID;
	  if (pipe7.getBitmapId() != targetId) {
		  pipe7.setBitmap(Bitmap(targetId));
		  pipe7.invalidate();
	  }

	  if ( steamActive != turbineHead.isAnimatedImageRunning() )
		  turbineHead.pauseAnimation();

	  tickCounter = 0;
    }
}


void MainScreenView::updateCurrentTime(Tempo tempo)
{
	Unicode::snprintf(dateTextBuffer, DATETEXT_SIZE, "%02u/%02u/%04u", tempo.day, tempo.month, tempo.year);
	dateText.invalidate();

	Unicode::snprintf(clockTextBuffer, CLOCKTEXT_SIZE, "%02u:%02u:%02u", tempo.hour, tempo.min, tempo.sec);
	clockText.invalidate();
}

void MainScreenView::consolePrint()
{
	console.consolePrint();
}



