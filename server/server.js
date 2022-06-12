const opcua = require("node-opcua");
const { OPCUAServer, dataType, DataType, resolveNodeId, AttributeIds } = require("node-opcua");
const { DataSetFieldContentMask, JsonDataSetMessageContentMask, JsonNetworkMessageContentMask, BrokerTransportQualityOfService, PublishedDataItemsDataType, PubSubConfigurationDataType } = require("node-opcua-types");

const os = require("os");

function available_memory() {
    // var value = process.memoryUsage().heapUsed / 1000000;
    const percentageMemUsed = os.freemem() / os.totalmem() * 100.0;
    return percentageMemUsed;
}

(async () => {
    try {

        const server = new OPCUAServer({
            port: 4840,
            resourcePath: "",
            hostname: "192.168.1.33",
            buildInfo: {
                productName: "NodeServer",
                buildDate: new Date(),
            }
        });

        await server.initialize();


        //"add a temperature sensor"

        const addressSpace = server.engine.addressSpace;//GESTIONE DEI NODI
        const namespace = addressSpace.getOwnNamespace();

        const devices = namespace.addFolder("ObjectsFolder", { //Folder
            browseName: "Devices"
        });

        const sensor = namespace.addObject({ //Object
            organizedBy: addressSpace.rootFolder.objects.devices, //collega il sensore alla cartella devices
            browseName: "MySensor"
        })

        const model = namespace.addVariable({ //Variable
            propertyOf: sensor,
            browseName: "ModelSensor",
            dataType: "String",
            value: {
                get: function () {
                    return new opcua.Variant({
                        dataType: opcua.DataType.String,//BUILT-IN DATA TYPE
                        value: "One Way Sensor"
                    })
                }
            }
        });

        const free_memory = namespace.addVariable({ //Variable
            componentOf: sensor,
            browseName: "FreeMemory",
            description:
            {
                locale: "it-IT",
                text: "Memoria disponibile"
            },
            nodeId: "s=Memory", //IDENTIFICATORE CON STRINGA
            dataType: "Double",
            value: {
                get: () => new opcua.Variant({ dataType: DataType.Double, value: available_memory() })
            }
        });

        await server.start();

        console.log("server started at ", server.getEndpointUrl());
    } catch (err) {
        console.log(err);
        process.exit(1);
    }
})();

