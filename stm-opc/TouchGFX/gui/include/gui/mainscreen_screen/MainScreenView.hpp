#ifndef MAINSCREENVIEW_HPP
#define MAINSCREENVIEW_HPP

#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

extern "C" 
{
#include "opcua.h"
}

class MainScreenView : public MainScreenViewBase
{
public:
    MainScreenView();
    virtual ~MainScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void getTime();
    void updateTime(Tempo tempo);
    void updateGauge(double availMemory);

protected:
};

#endif // MAINSCREENVIEW_HPP
