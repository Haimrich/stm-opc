#ifndef DEBUGSCREENVIEW_HPP
#define DEBUGSCREENVIEW_HPP

#include <gui_generated/debugscreen_screen/DebugScreenViewBase.hpp>
#include <gui/debugscreen_screen/DebugScreenPresenter.hpp>

class DebugScreenView : public DebugScreenViewBase
{
public:
    DebugScreenView();
    virtual ~DebugScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void getTime();
    void updateTime(Tempo tempo);
    void updateGauge(double availMemory);

protected:
};

#endif // DEBUGSCREENVIEW_HPP
