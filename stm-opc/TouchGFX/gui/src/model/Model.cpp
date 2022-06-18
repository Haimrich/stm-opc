#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

extern "C" 
{
#include "opcua.h"
#include "cmsis_os.h"
}

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
	uint8_t status;
	if (osMessageQueueGet(opcuaResultQueueHandle, &status, NULL, 0) == osOK) {

		double dValue;
		bool bValue;
		Tempo tempo;

		osMutexAcquire(opcuaMutexHandle, osWaitForever);
        switch(status) {
            case OPCUA_NEW_TIME_VALUE:
                tempo = gData.opcuaTime;
                osMutexRelease(opcuaMutexHandle);
                modelListener->updateTime(tempo);
                break;
            case OPCUA_NEW_AVAILABLE_MEMORY_VALUE:
                dValue = gData.availableMemory;
                osMutexRelease(opcuaMutexHandle);
                modelListener->updateMemoryGauge(dValue);
                break;
            case OPCUA_NEW_MAINTANKTEMP:
			    dValue = gData.mainTankTemperature;
			    osMutexRelease(opcuaMutexHandle);
			    modelListener->updateMainTankTemp(dValue);
			    break;
            case OPCUA_NEW_MAINTANKLEVEL:
			    dValue = gData.mainTankLevel;
			    osMutexRelease(opcuaMutexHandle);
			    modelListener->updateMainTankLevel(dValue);
			    break;
            case OPCUA_NEW_BOILERTEMP:
			    dValue = gData.boilerTemperature;
			    osMutexRelease(opcuaMutexHandle);
			    modelListener->updateBoilerTemp(dValue);
			    break;
            case OPCUA_NEW_BOILERLEVEL:
			    dValue = gData.boilerLevel;
			    osMutexRelease(opcuaMutexHandle);
			    modelListener->updateBoilerLevel(dValue);
			    break;
            case OPCUA_NEW_BOILERPRESSURE:
			    dValue = gData.boilerPressure;
			    osMutexRelease(opcuaMutexHandle);
			    modelListener->updateBoilerPressure(dValue);
			    break;
            case OPCUA_NEW_TURBINEPOWER:
			    dValue = gData.turbinePower;
			    osMutexRelease(opcuaMutexHandle);
			    modelListener->updateTurbinePower(dValue);
			    break;
            case OPCUA_NEW_VALVESTATUS:
			    bValue = gData.valveStatus;
			    osMutexRelease(opcuaMutexHandle);
			    modelListener->updateValveStatus(bValue);
			    break;
            case OPCUA_NEW_PUMPSTATUS:
			    bValue = gData.pumpStatus;
			    osMutexRelease(opcuaMutexHandle);
			    modelListener->updatePumpStatus(bValue);
			    break;
            case OPCUA_NEW_FIRESTATUS:
			    bValue = gData.fireStatus;
			    osMutexRelease(opcuaMutexHandle);
			    modelListener->updateFireStatus(bValue);
			    break;
            case OPCUA_NEW_CURRENTTIME:
            	tempo = gData.currentTime;
				osMutexRelease(opcuaMutexHandle);
				modelListener->updateCurrentTime(tempo);
				break;
            case CONSOLE_NEW_MESSAGE:
				osMutexRelease(opcuaMutexHandle);
				modelListener->consolePrint();
				break;
            default:
            	osMutexRelease(opcuaMutexHandle);
                break;
        }
	}
}
