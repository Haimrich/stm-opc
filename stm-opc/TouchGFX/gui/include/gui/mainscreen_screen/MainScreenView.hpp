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

    void handleTickEvent();

    void valveClickHandler(const Image& img, const ClickEvent& e);
    void pumpClickHandler(const Image& img, const ClickEvent& e);
    void fireClickHandler(const Image& img, const ClickEvent& e);

    void updateMainTankTemperature(double temp);
    void updateMainTankLevel(double level);

    void updateBoilerTemperature(double temp);
    void updateBoilerLevel(double level);
    void updateBoilerPressure(double pressure);

    void updateTurbinePower(double power);

    void updateValveStatus(bool status);
    void updatePumpStatus(bool status);
    void updateFireStatus(bool status);

    void updateCurrentTime(Tempo tempo);
    void consolePrint();

protected:

    Callback<MainScreenView, const Image&, const ClickEvent&> valveClickedCallback;
    Callback<MainScreenView, const Image&, const ClickEvent&> pumpClickedCallback;
    Callback<MainScreenView, const Image&, const ClickEvent&> fireClickedCallback;

    bool valveStatus;
    bool pumpStatus;
    bool fireStatus;

    int tickCounter;
};

#endif // MAINSCREENVIEW_HPP
