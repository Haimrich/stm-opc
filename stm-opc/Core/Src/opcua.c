#include "opcua.h"
#include "open62541.h"
#include "cmsis_os.h"

// Shared Global Variables between tasks
double gOpcuaAvailableMemory;
Tempo gOpcuaTime;

#define OPCUA_SERVER_ENDPOINT "opc.tcp://192.168.1.33:4840"

static void
handler_availMemoryChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                           UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "availableMemory has changed!");

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
    	UA_Double double_data = *((UA_Double *)value->value.data);
    	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Available memory is %.2f%%", double_data);

    	osMutexAcquire(opcuaMutexHandle, osWaitForever);
    	gOpcuaAvailableMemory = double_data;
    	osMutexRelease(opcuaMutexHandle);
    	uint8_t status = OPCUA_NEW_AVAILABLE_MEMORY_VALUE;
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
        /* A new session was created. We need to create the subscription. */
        /* Create a subscription */
        UA_CreateSubscriptionRequest request = UA_CreateSubscriptionRequest_default();
        UA_CreateSubscriptionResponse response =
            UA_Client_Subscriptions_create(client, request, NULL, NULL, deleteSubscriptionCallback);
            if(response.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Create subscription succeeded, id %u",
                            response.subscriptionId);
            else
                return;

            /* Add a MonitoredItem */
            UA_NodeId availMemoryNode = UA_NODEID_STRING(1, "AvailableMemory");

            UA_MonitoredItemCreateRequest monRequest =
            	UA_MonitoredItemCreateRequest_default(availMemoryNode);

            UA_MonitoredItemCreateResult monResponse =
                UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId,
                                                          UA_TIMESTAMPSTORETURN_BOTH, monRequest,
                                                          NULL, handler_availMemoryChanged, NULL);
            if(monResponse.statusCode == UA_STATUSCODE_GOOD)
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Monitoring n=1,s=AvailableMemory', id %lu", monResponse.monitoredItemId);
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

    	uint8_t status = 0;

		if (osMessageQueueGet(opcuaRequestQueueHandle, &status, NULL, 0) == osOK)
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
			gOpcuaTime = tempo;
			osMutexRelease(opcuaMutexHandle);
			status = OPCUA_NEW_TIME_VALUE;
			osMessageQueuePut(opcuaResultQueueHandle, &status, 0, osWaitForever);
		}

		UA_Client_run_iterate(client, 1000);
	}


	UA_Client_delete(client);
}
