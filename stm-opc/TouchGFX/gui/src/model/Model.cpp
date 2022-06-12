#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

extern "C" 
{
#include "opcua.h"
#include "cmsis_os.h"
extern osMessageQueueId_t opcuaResultQueueHandle;
}

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
	Tempo tempo;
	if (osMessageQueueGet(opcuaResultQueueHandle, &tempo, NULL, 0) == osOK) {
		modelListener->updateTime(tempo);
	}
}
