#include <gui/debugscreen_screen/DebugScreenView.hpp>
#include <gui/debugscreen_screen/DebugScreenPresenter.hpp>

extern "C"
{
#include "opcua.h"
}

DebugScreenPresenter::DebugScreenPresenter(DebugScreenView& v)
    : view(v)
{

}

void DebugScreenPresenter::activate()
{

}

void DebugScreenPresenter::deactivate()
{

}

void DebugScreenPresenter::updateTime(Tempo tempo)
{
	view.updateTime(tempo);
}

void DebugScreenPresenter::updateMemoryGauge(double availMemory)
{
	view.updateGauge(availMemory);
}
