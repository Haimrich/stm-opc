#ifndef MAINSCREENPRESENTER_HPP
#define MAINSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

extern "C" 
{
#include "opcua.h"
}

using namespace touchgfx;

class MainScreenView;

class MainScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    MainScreenPresenter(MainScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~MainScreenPresenter() {};

    void updateMainTankTemp(double value);
    void updateMainTankLevel(double value);
    void updateBoilerTemp(double value);
    void updateBoilerLevel(double value);
    void updateBoilerPressure(double value);
    void updateTurbinePower(double value);
    void updateValveStatus(bool value);
    void updatePumpStatus(bool value);
    void updateFireStatus(bool value);

    void updateCurrentTime(Tempo tempo);
    void consolePrint();

private:
    MainScreenPresenter();

    MainScreenView& view;
};

#endif // MAINSCREENPRESENTER_HPP
