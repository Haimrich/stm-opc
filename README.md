# OPC UA Client on STM32F769I-DISCO Board

Industrial Informatics Course Project - UniCT 2022

![Board Gif](https://user-images.githubusercontent.com/7826610/174495418-a4c22a68-e661-4564-a424-82a226189bd9.gif)

This is a HMI Proof-of-Concept for an industrial system built using TouchGFX Designer and the OPC UA library [open62541](https://github.com/open62541/open62541) on a STM32F769I-DISCO board. Tank fill levels, temperatures, pressure and turbine power are continously updated using an OPC UA subscription. The valve, pump and fire can be turned on and off by pressing them and causing a value write on the OPC UA server. All the project files are available [here](/stm-opc).

The OPC UA test server was implemented in JavaScript using [node-opcua](https://github.com/node-opcua/node-opcua). In a user namespace a Tank ObjectType was defined and two tanks, valve, pump and turbine objects were added. There is also some code to simulate the variation of varaibles.

## Tutorial

A tutorial in italian to build something similar is available [here](/TUTORIAL.IT.md).
