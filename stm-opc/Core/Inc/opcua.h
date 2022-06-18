#ifndef OPCUA_H_
#define OPCUA_H_

#include "stdint.h"
#include "stdbool.h"
#include "cmsis_os.h"

typedef struct Tempo {
	uint16_t day;
	uint16_t month;
	uint16_t year;

	uint16_t hour;
	uint16_t min;
	uint16_t sec;
} Tempo;


extern osMessageQueueId_t opcuaRequestQueueHandle;
extern osMessageQueueId_t opcuaResultQueueHandle;

extern osMutexId_t opcuaMutexHandle;

#define CONSOLE_BUFFER_SIZE 148

typedef struct GlobalData {
	// DebugScreen

	double availableMemory;
	Tempo opcuaTime;

	// MainScreen

	bool valveStatus;
	bool pumpStatus;
	bool fireStatus;

	double mainTankLevel;
	double mainTankTemperature;

	double boilerLevel;
	double boilerTemperature;
	double boilerPressure;

	double turbinePower;

	char consoleBuffer[CONSOLE_BUFFER_SIZE];
	Tempo currentTime;
} GlobalData;

extern GlobalData gData;

#define OPCUA_NEW_AVAILABLE_MEMORY_VALUE 	0
#define OPCUA_NEW_TIME_VALUE 				1
#define OPCUA_NEW_MAINTANKTEMP				2
#define OPCUA_NEW_MAINTANKLEVEL				3
#define OPCUA_NEW_BOILERTEMP				4
#define OPCUA_NEW_BOILERLEVEL				5
#define OPCUA_NEW_BOILERPRESSURE			6
#define OPCUA_NEW_TURBINEPOWER				7
#define OPCUA_NEW_VALVESTATUS				8
#define OPCUA_NEW_PUMPSTATUS				9
#define OPCUA_NEW_FIRESTATUS				10
#define OPCUA_NEW_CURRENTTIME				11
#define CONSOLE_NEW_MESSAGE					12

#define OPCUA_REQUEST_TIME					0
#define OPCUA_REQUEST_SWITCH_VALVE			1
#define OPCUA_REQUEST_SWITCH_PUMP			2
#define OPCUA_REQUEST_SWITCH_FIRE			3

#endif
