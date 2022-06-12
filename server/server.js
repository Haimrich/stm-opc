const opcua = require("node-opcua");
const { OPCUAServer, dataType, DataType, resolveNodeId, AttributeIds } = require("node-opcua");
const { DataSetFieldContentMask, JsonDataSetMessageContentMask, JsonNetworkMessageContentMask, BrokerTransportQualityOfService, PublishedDataItemsDataType, PubSubConfigurationDataType } = require("node-opcua-types");

const os = require("os");

(async () => {
    try {

        const server = new OPCUAServer({
            port: 4840,
            resourcePath: "",
            buildInfo: {
                productName: "OpcUa Server",
                buildDate: new Date(),
            }
        });

        await server.initialize();

        const addressSpace = server.engine.addressSpace;
        const namespace = addressSpace.getOwnNamespace();

        //Folder
        const myFolder = namespace.addFolder("ObjectsFolder", {
            browseName: "MyFolder"
        });

        //Object
        const serverStats = namespace.addObject({
            organizedBy: addressSpace.rootFolder.objects.myFolder,
            browseName: "ServerStats"
        })

        // Variable
        const freeMemory = namespace.addVariable({ //Variable
            componentOf: serverStats,
            browseName: "AvaiableMemory",
            description:
            {
                locale: "it-IT",
                text: "Memoria disponibile nel server"
            },
            nodeId: "s=AvailableMemory",
            dataType: "Double",
            value: {
                refreshFunc: function (callback) {
                    let dataValue = new opcua.DataValue({
                        value: new opcua.Variant({ 
                            dataType: opcua.DataType.Double, 
                            value: available_memory()
                        }),
                        sourceTimestamp: new Date()
                    });
                    callback(null, dataValue);
                }
            }
        });
        
        setInterval(() => {
            const availableMemory = available_memory();
            freeMemory.setValueFromSource({ dataType: "Double", value: availableMemory });
        }, 1000);

        await server.start();
        console.log("Server Started at: ", server.getEndpointUrl());

    } catch (err) {
        console.log(err);
        process.exit(1);
    }
})();

function available_memory() {
    const percentageMemUsed = os.freemem() / os.totalmem() * 100.0;
    return percentageMemUsed;
}
