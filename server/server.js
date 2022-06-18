const opcua = require("node-opcua");
const { OPCUAServer, Variant, DataType } = require("node-opcua");

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

        // Main Folder

        const myFolder = namespace.addFolder("ObjectsFolder", {
            browseName: "MyFolder"
        });

        // ServerStats Object

        const serverStats = namespace.addObject({
            organizedBy: addressSpace.rootFolder.objects.myFolder,
            browseName: "ServerStats"
        })

        // Free Memory Variable

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

        // Free Memory "Sensor" Simulation

        setInterval(() => {
            const availableMemory = available_memory();
            freeMemory.setValueFromSource({ dataType: "Double", value: availableMemory });
        }, 1000);


        // System Simulation

        let mainTankTemperature = 26.5;
        let mainTankLevel = 0.5;

        let boilerTankTemperature = 40.5;
        let boilerTankLevel = 0.3;
        let boilerTankPressure = 1;

        let valveStatus = false;
        let pumpStatus = false;
        let fireStatus = true;

        let turbinePower = 100;

        setInterval(() => {
            if ( valveStatus == true ) {
                mainTankLevel += 0.02 + Math.random() * 0.005;
            }

            if ( (pumpStatus == true) && (mainTankLevel > 0) ) {
                const diff = 0.015 + Math.random() * 0.005;
                mainTankLevel -= diff;
                boilerTankLevel += diff;
            }

            if ( fireStatus == true ) {
                boilerTankLevel -= 0.012 + Math.random() * 0.005;
            }

            mainTankLevel = Math.min(mainTankLevel, 1);
            mainTankLevel = Math.max(mainTankLevel, 0);
            
            boilerTankLevel = Math.min(boilerTankLevel, 1);
            boilerTankLevel = Math.max(boilerTankLevel, 0);
            
            const boilerActive = fireStatus && (boilerTankLevel > 0);

            const boilerTankPressureTarget = boilerActive ? 3.5 : 1.0;
            boilerTankPressure += (boilerTankPressureTarget - boilerTankPressure)*0.04 + Math.random() * 0.01;

            const boilerTankTemperatureTarget = boilerActive ? 256 : mainTankTemperature;
            boilerTankTemperature += (boilerTankTemperatureTarget - boilerTankTemperature) * 0.05 + Math.random() * 0.01;

            const turbinePowerTarget = boilerActive ? 750 : 0;
            const turbinePowerOscillation = boilerActive ?  Math.random() * 5 : 0;
            turbinePower += (turbinePowerTarget - turbinePower) * 0.05 + turbinePowerOscillation;
        }, 500);


        setInterval(() => {
            mainTankTemperature += (0.5 - Math.random()) * 0.3
            if (mainTankTemperature < 17) mainTankTemperature = 17
            if (mainTankTemperature > 39) mainTankTemperature = 39

        }, 2000);

        // Tank ObjectType

        const tankType = namespace.addObjectType({
            browseName: "TankType"
        });
        namespace.addVariable({
            propertyOf: tankType,
            browseName: "IdNumber",
            description: "The identification number of the tank",
            dataType: "UInt16",
            modellingRule: "Optional"
        });
        namespace.addVariable({
            componentOf: tankType,
            browseName: "Temperature",
            description: "The temperature value (in degree C) measured in the tank",
            dataType: "Double",
            modellingRule: "Mandatory"
        });
        namespace.addVariable({
            componentOf: tankType,
            browseName: "Level",
            description: "The percentage fill level of the tank",
            dataType: "Double",
            modellingRule: "Mandatory"
        });
        namespace.addVariable({
            componentOf: tankType,
            browseName: "Pressure",
            description: "The tank pressure measured in bar",
            dataType: "Double",
            modellingRule: "Optional"
        });

        // System Object

        let systemObject = namespace.addObject({
            organizedBy: myFolder,
            browseName: "System"
        })

        // Tank Objects Instantiation

        let mainTankObject = tankType.instantiate({
            browseName: "MainTank",
            componentOf: systemObject
        });

        mainTankObject.temperature.bindVariable({
            get: () => {
                return new Variant({ dataType: DataType.Double, value: mainTankTemperature });
            },
            set: (variant) => {
                mainTankTemperature = parseFloat(variant.value);
                return StatusCodes.Good;
            },
        }, true);

        mainTankObject.level.bindVariable({
            get: () => {
                return new Variant({ dataType: DataType.Double, value: mainTankLevel });
            },
            set: (variant) => {
                mainTankLevel = parseFloat(variant.value);
                return StatusCodes.Good;
            },
        }, true);

        let boilerTankObject = tankType.instantiate({
            componentOf: systemObject,
            browseName: "BoilerTank",
            optionals: ["Pressure"]
        });

        boilerTankObject.temperature.bindVariable({
            get: () => {
                return new Variant({ dataType: DataType.Double, value: boilerTankTemperature });
            },
            set: (variant) => {
                boilerTankTemperature = parseFloat(variant.value);
                return StatusCodes.Good;
            },
        }, true);

        boilerTankObject.level.bindVariable({
            get: () => {
                return new Variant({ dataType: DataType.Double, value: boilerTankLevel });
            },
            set: (variant) => {
                boilerTankLevel = parseFloat(variant.value);
                return StatusCodes.Good;
            },
        }, true);

        boilerTankObject.pressure.bindVariable({
            get: () => {
                return new Variant({ dataType: DataType.Double, value: boilerTankPressure });
            },
            set: (variant) => {
                boilerTankPressure = parseFloat(variant.value);
                return StatusCodes.Good;
            },
        }, true);

        // Valve, Pump and Fire status Variables

        let valveStatusVariable = namespace.addVariable({
            componentOf: systemObject,
            browseName: "ValveStatus",
            description: "Valve status (ON = true, OFF = false)",
            dataType: "Boolean",
            value: {
                get: function () {
                    return new opcua.Variant({ dataType: opcua.DataType.Boolean, value: valveStatus });
                },
                set: function (variant) {
                    valveStatus = (variant.value === true);
                    return opcua.StatusCodes.Good;
                }
            }
        });

        let pumpStatusVariable = namespace.addVariable({
            componentOf: systemObject,
            browseName: "PumpStatus",
            description: "Pump status (ON = true, OFF = false)",
            dataType: "Boolean",
            value: {
                get: function () {
                    return new opcua.Variant({ dataType: opcua.DataType.Boolean, value: pumpStatus });
                },
                set: function (variant) {
                    pumpStatus = (variant.value === true);
                    return opcua.StatusCodes.Good;
                }
            }
        });

        let fireStatusVariable = namespace.addVariable({
            componentOf: systemObject,
            browseName: "FireStatus",
            description: "Fire status (ON = true, OFF = false)",
            dataType: "Boolean",
            value: {
                get: function () {
                    return new opcua.Variant({ dataType: opcua.DataType.Boolean, value: fireStatus });
                },
                set: function (variant) {
                    fireStatus = (variant.value === true);
                    return opcua.StatusCodes.Good;
                }
            }
        });

        // Turbine Object

        let turbineObject = namespace.addObject({
            componentOf: systemObject,
            browseName: "Turbine"
        });

        let turbinePowerVariable = namespace.addVariable({
            componentOf: turbineObject,
            browseName: "Power",
            description: "Turbine energy production power measured in KW",
            dataType: "Double",
            value: {
                get: function () {
                    return new opcua.Variant({ dataType: opcua.DataType.Double, value: turbinePower });
                },
                set: function (variant) {
                    turbinePower = parseFloat(variant.value);
                    return opcua.StatusCodes.Good;
                }
            }
        });





        // Server Start

        await server.start();
        console.log("Server Started at: ", server.getEndpointUrl());

    } catch (err) {
        console.log(err);
        process.exit(1);
    }
})();

// Utility Functions

function available_memory() {
    const percentageMemUsed = os.freemem() / os.totalmem() * 100.0;
    return percentageMemUsed;
}
