#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>


MainScreenPresenter::MainScreenPresenter(MainScreenView& v)
    : view(v)
{

}

void MainScreenPresenter::activate()
{

}

void MainScreenPresenter::deactivate()
{

}

void MainScreenPresenter::updateMainTankTemp(double value)
{
	view.updateMainTankTemperature(value);
}

void MainScreenPresenter::updateMainTankLevel(double value)
{
	view.updateMainTankLevel(value);
}

void MainScreenPresenter::updateBoilerTemp(double value)
{
	view.updateBoilerTemperature(value);
}

void MainScreenPresenter::updateBoilerLevel(double value)
{
	view.updateBoilerLevel(value);
}

void MainScreenPresenter::updateBoilerPressure(double value)
{
	view.updateBoilerPressure(value);
}

void MainScreenPresenter::updateTurbinePower(double value)
{
	view.updateTurbinePower(value);
}

void MainScreenPresenter::updateValveStatus(bool value)
{
	view.updateValveStatus(value);
}

void MainScreenPresenter::updatePumpStatus(bool value)
{
	view.updatePumpStatus(value);
}

void MainScreenPresenter::updateFireStatus(bool value)
{
	view.updateFireStatus(value);
}

void MainScreenPresenter::updateCurrentTime(Tempo tempo)
{
	view.updateCurrentTime(tempo);
}

void MainScreenPresenter::consolePrint()
{
	view.consolePrint();
}

