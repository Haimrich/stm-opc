#include "opcua.h"
#include "open62541.h"
#include "cmsis_os.h"

// Shared Global Variables between tasks
GlobalData gData;

#define OPCUA_SERVER_ENDPOINT "opc.tcp://192.168.1.33:4840"

static void
handler_availMemoryChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "availableMemory has changed!");

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
    	UA_Double double_data = *((UA_Double *)value->value.data);
    	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Available memory is %.2f%%", double_data);

    	osMutexAcquire(opcuaMutexHandle, osWaitForever);
    	gData.availableMemory = double_data;
    	osMutexRelease(opcuaMutexHandle);
    	uint8_t status = OPCUA_NEW_AVAILABLE_MEMORY_VALUE;
    	osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
    }
}

static void
handler_MainTankTemperatureChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "MainTankTemperature has changed!");

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
    	UA_Double double_data = *((UA_Double *)value->value.data);
    	osMutexAcquire(opcuaMutexHandle, osWaitForever);
    	gData.mainTankTemperature = double_data;
    	osMutexRelease(opcuaMutexHandle);
    	uint8_t status = OPCUA_NEW_MAINTANKTEMP;
    	osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
    }
}

static void
handler_MainTankLevelChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "MainTankLevel has changed!");

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
    	UA_Double double_data = *((UA_Double *)value->value.data);
    	osMutexAcquire(opcuaMutexHandle, osWaitForever);
    	gData.mainTankLevel = double_data;
    	osMutexRelease(opcuaMutexHandle);
    	uint8_t status = OPCUA_NEW_MAINTANKLEVEL;
    	osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
    }
}

static void
handler_BoilerTemperatureChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "BoilerTankTemperature has changed!");

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
    	UA_Double double_data = *((UA_Double *)value->value.data);
    	osMutexAcquire(opcuaMutexHandle, osWaitForever);
    	gData.boilerTemperature = double_data;
    	osMutexRelease(opcuaMutexHandle);
    	uint8_t status = OPCUA_NEW_BOILERTEMP;
    	osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
    }
}

static void
handler_BoilerLevelChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "BoilerTankLevel has changed!");

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
    	UA_Double double_data = *((UA_Double *)value->value.data);
    	osMutexAcquire(opcuaMutexHandle, osWaitForever);
    	gData.boilerLevel = double_data;
    	osMutexRelease(opcuaMutexHandle);
    	uint8_t status = OPCUA_NEW_BOILERLEVEL;
    	osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
    }
}

static void
handler_BoilerPressureChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "BoilerTankPressure has changed!");

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
    	UA_Double double_data = *((UA_Double *)value->value.data);
    	osMutexAcquire(opcuaMutexHandle, osWaitForever);
    	gData.boilerPressure = double_data;
    	osMutexRelease(opcuaMutexHandle);
    	uint8_t status = OPCUA_NEW_BOILERPRESSURE;
    	osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
    }
}

static void
handler_TurbinePowerChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "TurbinePower has changed!");

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
    	UA_Double double_data = *((UA_Double *)value->value.data);
    	osMutexAcquire(opcuaMutexHandle, osWaitForever);
    	gData.turbinePower = double_data;
    	osMutexRelease(opcuaMutexHandle);
    	uint8_t status = OPCUA_NEW_TURBINEPOWER;
    	osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
    }
}

static void
handler_ValveStatusChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "ValveStatus has changed!");

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_BOOLEAN])) {
    	UA_Boolean bool_data = *((UA_Boolean *)value->value.data);
    	osMutexAcquire(opcuaMutexHandle, osWaitForever);
    	gData.valveStatus = bool_data;
    	osMutexRelease(opcuaMutexHandle);
    	uint8_t status = OPCUA_NEW_VALVESTATUS;
    	osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
    }
}

static void
handler_PumpStatusChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "PumpStatus has changed!");

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_BOOLEAN])) {
    	UA_Boolean bool_data = *((UA_Boolean *)value->value.data);
    	osMutexAcquire(opcuaMutexHandle, osWaitForever);
    	gData.pumpStatus = bool_data;
    	osMutexRelease(opcuaMutexHandle);
    	uint8_t status = OPCUA_NEW_PUMPSTATUS;
    	osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
    }
}

static void
handler_FireStatusChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "FireStatus has changed!");

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_BOOLEAN])) {
    	UA_Boolean bool_data = *((UA_Boolean *)value->value.data);
    	osMutexAcquire(opcuaMutexHandle, osWaitForever);
    	gData.fireStatus = bool_data;
    	osMutexRelease(opcuaMutexHandle);
    	uint8_t status = OPCUA_NEW_FIRESTATUS;
    	osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
    }
}

static void
handler_CurrentTimeChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "CurrentTime has changed!");

	if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DATETIME])) {
		UA_DateTime raw_date = *(UA_DateTime *) value->value.data;
		UA_DateTimeStruct dts = UA_DateTime_toStruct(raw_date);
		osMutexAcquire(opcuaMutexHandle, osWaitForever);
		gData.currentTime.day = dts.day;
		gData.currentTime.month = dts.month;
		gData.currentTime.year = dts.year;
		gData.currentTime.hour = dts.hour;
		gData.currentTime.min = dts.min;
		gData.currentTime.sec = dts.sec;
		osMutexRelease(opcuaMutexHandle);
		uint8_t status = OPCUA_NEW_CURRENTTIME;
		osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
	}
}


static void
deleteSubscriptionCallback(UA_Client *client, UA_UInt32 subscriptionId, void *subscriptionContext) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "Subscription Id %lu was deleted", subscriptionId);
}

static void
subscriptionInactivityCallback (UA_Client *client, UA_UInt32 subId, void *subContext) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Inactivity for subscription %lu", subId);
}

static void
stateCallback(UA_Client *client, UA_SecureChannelState channelState,
              UA_SessionState sessionState, UA_StatusCode recoveryStatus) {
    switch(sessionState) {
    case UA_SESSIONSTATE_ACTIVATED: {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "A session with the server is activated");

        UA_CreateSubscriptionRequest request;
        UA_CreateSubscriptionResponse response;

		UA_NodeId nodeId;
		UA_MonitoredItemCreateRequest monRequest;
		UA_MonitoredItemCreateResult monResponse;

        // Create Debug Subscription
#if false
        request = UA_CreateSubscriptionRequest_default();
        response = UA_Client_Subscriptions_create(client, request, NULL, NULL, deleteSubscriptionCallback);

		if(response.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Create subscription succeeded, id %lu", response.subscriptionId);
		else
			return;

		nodeId = UA_NODEID_STRING(1, "AvailableMemory");
		monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
		monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_BOTH, monRequest, NULL, handler_availMemoryChanged, NULL);
		if(monResponse.statusCode == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring n=1,s=AvailableMemory', id %lu", monResponse.monitoredItemId);

#endif
    	// Create System Status SubScription

    	request = UA_CreateSubscriptionRequest_default();
    	response = UA_Client_Subscriptions_create(client, request, NULL, NULL, deleteSubscriptionCallback);

		if(response.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Create subscription succeeded, id %lu", response.subscriptionId);
		else
			return;

		/* Add monitored items  */

		// Main Tank Temperature
		nodeId = UA_NODEID_NUMERIC(1, 1009);
		monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
		monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_NEITHER, monRequest, NULL, handler_MainTankTemperatureChanged, NULL);
		if(monResponse.statusCode == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring node (n=1,i=%lu). Monitored Item Id: %lu", nodeId.identifier.numeric, monResponse.monitoredItemId);

		// Main Tank Level
		nodeId = UA_NODEID_NUMERIC(1, 1010);
		monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
		monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_NEITHER, monRequest, NULL, handler_MainTankLevelChanged, NULL);
		if(monResponse.statusCode == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring node (n=1,i=%lu). Monitored Item Id: %lu", nodeId.identifier.numeric, monResponse.monitoredItemId);

		// Boiler Tank Temperature
		nodeId = UA_NODEID_NUMERIC(1, 1012);
		monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
		monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_NEITHER, monRequest, NULL, handler_BoilerTemperatureChanged, NULL);
		if(monResponse.statusCode == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring node (n=1,i=%lu). Monitored Item Id: %lu", nodeId.identifier.numeric, monResponse.monitoredItemId);

		// Boiler Tank Level
		nodeId = UA_NODEID_NUMERIC(1, 1013);
		monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
		monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_NEITHER, monRequest, NULL, handler_BoilerLevelChanged, NULL);
		if(monResponse.statusCode == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring node (n=1,i=%lu). Monitored Item Id: %lu", nodeId.identifier.numeric, monResponse.monitoredItemId);

		// Boiler Tank Pressure
		nodeId = UA_NODEID_NUMERIC(1, 1014);
		monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
		monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_NEITHER, monRequest, NULL, handler_BoilerPressureChanged, NULL);
		if(monResponse.statusCode == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring node (n=1,i=%lu). Monitored Item Id: %lu", nodeId.identifier.numeric, monResponse.monitoredItemId);

		// Turbine Power
		nodeId = UA_NODEID_NUMERIC(1, 1019);
		monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
		monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_NEITHER, monRequest, NULL, handler_TurbinePowerChanged, NULL);
		if(monResponse.statusCode == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring node (n=1,i=%lu). Monitored Item Id: %lu", nodeId.identifier.numeric, monResponse.monitoredItemId);

		// Fire Status
		nodeId = UA_NODEID_NUMERIC(1, 1017);
		monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
		monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_NEITHER, monRequest, NULL, handler_FireStatusChanged, NULL);
		if(monResponse.statusCode == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring node (n=1,i=%lu). Monitored Item Id: %lu", nodeId.identifier.numeric, monResponse.monitoredItemId);

		// Valve Status
		nodeId = UA_NODEID_NUMERIC(1, 1015);
		monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
		monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_NEITHER, monRequest, NULL, handler_ValveStatusChanged, NULL);
		if(monResponse.statusCode == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring node (n=1,i=%lu). Monitored Item Id: %lu", nodeId.identifier.numeric, monResponse.monitoredItemId);

		// Pump Status
		nodeId = UA_NODEID_NUMERIC(1, 1016);
		monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
		monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_NEITHER, monRequest, NULL, handler_PumpStatusChanged, NULL);
		if(monResponse.statusCode == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring node (n=1,i=%lu). Monitored Item Id: %lu", nodeId.identifier.numeric, monResponse.monitoredItemId);

		// Current Time for Main Screen
		nodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_SERVERSTATUS_CURRENTTIME);
		monRequest = UA_MonitoredItemCreateRequest_default(nodeId);
		monRequest.requestedParameters.samplingInterval = 1000;
		monResponse = UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_NEITHER, monRequest, NULL, handler_CurrentTimeChanged, NULL);
		if(monResponse.statusCode == UA_STATUSCODE_GOOD)
			UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring node (n=1,i=%lu). Monitored Item Id: %lu", nodeId.identifier.numeric, monResponse.monitoredItemId);
            }
        break;
    case UA_SESSIONSTATE_CLOSED:
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Session disconnected");
        break;
    default:
        break;
    }
}

void StartOpcuaTask(void *argument) {
    UA_Client *client = UA_Client_new();
    UA_ClientConfig *client_config = UA_Client_getConfig(client);
    UA_ClientConfig_setDefault(client_config);

    client_config->stateCallback = stateCallback;
    client_config->subscriptionInactivityCallback = subscriptionInactivityCallback;


	for (;;) {
        UA_StatusCode retval = UA_Client_connect(client, OPCUA_SERVER_ENDPOINT);
        if(retval != UA_STATUSCODE_GOOD) {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Not connected. Retrying to connect in 1 second");
            osDelay(1000);
            continue;
        }

    	uint8_t message = 0;
		bool status;

		if (osMessageQueueGet(opcuaRequestQueueHandle, &message, NULL, 0) == osOK)
		{
			switch (message)
			{
			case OPCUA_REQUEST_TIME:
				{
					printf("Bottone Premuto.\n");
					Tempo tempo = (Tempo ) { 0, 0, 0 };

					UA_Variant value;
					UA_Variant_init(&value);

					const UA_NodeId nodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_SERVERSTATUS_CURRENTTIME);
					retval = UA_Client_readValueAttribute(client, nodeId, &value);

					if (retval == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_DATETIME])) {
						UA_DateTime raw_date = *(UA_DateTime*) value.data;
						UA_DateTimeStruct dts = UA_DateTime_toStruct(raw_date);
						tempo = (Tempo ) { dts.hour, dts.min, dts.sec + 1 };
						printf("date is: %u-%u-%u %u:%u:%u\n", dts.day, dts.month, dts.year, dts.hour, dts.min, dts.sec);
					}

					UA_Variant_clear(&value);

					osMutexAcquire(opcuaMutexHandle, osWaitForever);
					gData.opcuaTime = tempo;
					osMutexRelease(opcuaMutexHandle);
					message = OPCUA_NEW_TIME_VALUE;
					osMessageQueuePut(opcuaResultQueueHandle, &message, 0, osWaitForever);
				}
				break;
			case OPCUA_REQUEST_SWITCH_VALVE:
				{
					printf("Valvola premuta.\n");

					osMutexAcquire(opcuaMutexHandle, osWaitForever);
					status = gData.valveStatus;
					osMutexRelease(opcuaMutexHandle);

					const UA_NodeId nodeId = UA_NODEID_NUMERIC(1, 1015);
				    UA_Variant *myVariant = UA_Variant_new();
				    UA_Variant_setScalarCopy(myVariant, &status, &UA_TYPES[UA_TYPES_BOOLEAN]);
				    UA_Client_writeValueAttribute(client, nodeId, myVariant);
				    UA_Variant_delete(myVariant);
				}
				break;
			case OPCUA_REQUEST_SWITCH_PUMP:
				{
					printf("Pompa premuta.\n");

					osMutexAcquire(opcuaMutexHandle, osWaitForever);
					status = gData.pumpStatus;
					osMutexRelease(opcuaMutexHandle);

					const UA_NodeId nodeId = UA_NODEID_NUMERIC(1, 1016);
					UA_Variant *myVariant = UA_Variant_new();
					UA_Variant_setScalarCopy(myVariant, &status, &UA_TYPES[UA_TYPES_BOOLEAN]);
					UA_Client_writeValueAttribute(client, nodeId, myVariant);
					UA_Variant_delete(myVariant);
				}
				break;
			case OPCUA_REQUEST_SWITCH_FIRE:
				{
					printf("Fire premuto.\n");

					osMutexAcquire(opcuaMutexHandle, osWaitForever);
					status = gData.fireStatus;
					osMutexRelease(opcuaMutexHandle);

					const UA_NodeId nodeId = UA_NODEID_NUMERIC(1, 1017);
					UA_Variant *myVariant = UA_Variant_new();
					UA_Variant_setScalarCopy(myVariant, &status, &UA_TYPES[UA_TYPES_BOOLEAN]);
					UA_Client_writeValueAttribute(client, nodeId, myVariant);
					UA_Variant_delete(myVariant);
				}
				break;
			default:
				break;
			}
		}

		UA_Client_run_iterate(client, 1000);
	}


	UA_Client_delete(client);
}
