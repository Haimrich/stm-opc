/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef DEBUGSCREENVIEWBASE_HPP
#define DEBUGSCREENVIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/debugscreen_screen/DebugScreenPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/Button.hpp>
#include <touchgfx/containers/clock/DigitalClock.hpp>
#include <touchgfx/widgets/Gauge.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565.hpp>
#include <touchgfx/widgets/TextArea.hpp>

class DebugScreenViewBase : public touchgfx::View<DebugScreenPresenter>
{
public:
    DebugScreenViewBase();
    virtual ~DebugScreenViewBase() {}
    virtual void setupScreen();

    /*
     * Virtual Action Handlers
     */
    virtual void getTime()
    {
        // Override and implement this function in DebugScreen
    }

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box __background;
    touchgfx::Box backgroundBox;
    touchgfx::Button getTimeButton;
    touchgfx::DigitalClock digitalClock;
    touchgfx::Gauge gauge;
    touchgfx::PainterRGB565 gaugePainter;
    touchgfx::TextArea gaugeDescriptionText;

private:

    /*
     * Callback Declarations
     */
    touchgfx::Callback<DebugScreenViewBase, const touchgfx::AbstractButton&> buttonCallback;

    /*
     * Callback Handler Declarations
     */
    void buttonCallbackHandler(const touchgfx::AbstractButton& src);

    /*
     * Canvas Buffer Size
     */
    static const uint16_t CANVAS_BUFFER_SIZE = 12000;
    uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];
};

#endif // DEBUGSCREENVIEWBASE_HPP
