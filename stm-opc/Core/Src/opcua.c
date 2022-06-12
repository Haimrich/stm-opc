#include "opcua.h"
#include "open62541.h"
#include "cmsis_os.h"

extern osMessageQueueId_t opcuaRequestQueueHandle;
extern osMessageQueueId_t opcuaResultQueueHandle;

#define OPCUA_SERVER_ENDPOINT "opc.tcp://192.168.1.33:4840"

void StartOpcuaTask(void *argument) {
	Tempo tempo = (Tempo ) { 0, 0, 0 };
	uint8_t status = 0;

	for (;;) {
		if (osMessageQueueGet(opcuaRequestQueueHandle, &status, NULL, 0) == osOK)
		{
			printf("Bottone Premuto.\n");
			UA_Client *client = UA_Client_new();
			UA_ClientConfig_setDefault(UA_Client_getConfig(client));
			UA_StatusCode retval = UA_Client_connect(client, OPCUA_SERVER_ENDPOINT);

			if (retval != UA_STATUSCODE_GOOD) {
				UA_Client_delete(client);
				tempo.sec = 2;
			} else {
				UA_Variant value;
				UA_Variant_init(&value);

				const UA_NodeId nodeId = UA_NODEID_NUMERIC(0,
				UA_NS0ID_SERVER_SERVERSTATUS_CURRENTTIME);
				retval = UA_Client_readValueAttribute(client, nodeId, &value);

				if (retval == UA_STATUSCODE_GOOD
						&& UA_Variant_hasScalarType(&value,
								&UA_TYPES[UA_TYPES_DATETIME])) {
					UA_DateTime raw_date = *(UA_DateTime*) value.data;
					UA_DateTimeStruct dts = UA_DateTime_toStruct(raw_date);
					tempo = (Tempo ) { dts.hour, dts.min, dts.sec + 1 };
					printf("date is: %u-%u-%u %u:%u:%u\n", dts.day, dts.month, dts.year, dts.hour, dts.min, dts.sec);
				} else {
					tempo.sec = 3;
				}

				UA_Variant_clear(&value);
				UA_Client_delete(client);
			}

			osMessageQueuePut(opcuaResultQueueHandle, &tempo, 0, 0);
		}
		osDelay(10);
	}
}
