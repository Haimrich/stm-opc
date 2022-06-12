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
        switch(status) {
            case OPCUA_NEW_TIME_VALUE:
                Tempo tempo;
                osMutexAcquire(opcuaMutexHandle, osWaitForever);
                tempo = gOpcuaTime;
                osMutexRelease(opcuaMutexHandle);
                modelListener->updateTime(tempo);
                break;
            case OPCUA_NEW_AVAILABLE_MEMORY_VALUE:
                double mem;
                osMutexAcquire(opcuaMutexHandle, osWaitForever);
                mem = gOpcuaAvailableMemory;
                osMutexRelease(opcuaMutexHandle);
                modelListener->updateGauge(mem);
                break;
            default:
                break;
        }
	}
}
