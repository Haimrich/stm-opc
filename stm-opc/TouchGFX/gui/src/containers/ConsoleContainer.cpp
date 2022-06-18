#include <gui/containers/ConsoleContainer.hpp>

extern "C"
{
#include "opcua.h"
#include "cmsis_os.h"
}


ConsoleContainer::ConsoleContainer()
{
	consoleLines[0] = line1Buffer;
	consoleLines[1] = line2Buffer;
	consoleLines[2] = line3Buffer;
	consoleLines[3] = line4Buffer;
	consoleLines[4] = line5Buffer;
	consoleLines[5] = line6Buffer;
	consoleLines[6] = line7Buffer;
	consoleLines[7] = line8Buffer;
	consoleLines[8] = line9Buffer;
	consoleLines[9] = line10Buffer;
	consoleLines[10] = line11Buffer;
	consoleLines[11] = line12Buffer;
}

void ConsoleContainer::initialize()
{
    ConsoleContainerBase::initialize();
}

void ConsoleContainer::consolePrint()
{
	// Move all lines up
	for (unsigned i = 0; i < NUM_CONSOLE_LINES-1; i++)
		Unicode::strncpy(consoleLines[i], consoleLines[i+1], CONSOLE_BUFFER_SIZE);

	// Write last line
	osMutexAcquire(opcuaMutexHandle, osWaitForever);
	Unicode::strncpy(consoleLines[NUM_CONSOLE_LINES- 1], gData.consoleBuffer, CONSOLE_BUFFER_SIZE);
	osMutexRelease(opcuaMutexHandle);

	this->invalidate();
}
