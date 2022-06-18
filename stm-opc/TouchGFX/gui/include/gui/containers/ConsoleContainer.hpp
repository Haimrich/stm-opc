#ifndef CONSOLECONTAINER_HPP
#define CONSOLECONTAINER_HPP

#include <gui_generated/containers/ConsoleContainerBase.hpp>

#define NUM_CONSOLE_LINES 12

class ConsoleContainer : public ConsoleContainerBase
{
public:
    ConsoleContainer();
    virtual ~ConsoleContainer() {}

    virtual void initialize();

    void consolePrint();
protected:

    touchgfx::Unicode::UnicodeChar* consoleLines[NUM_CONSOLE_LINES];
};


#endif // CONSOLECONTAINER_HPP
