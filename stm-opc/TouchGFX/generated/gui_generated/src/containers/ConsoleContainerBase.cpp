/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/containers/ConsoleContainerBase.hpp>
#include <touchgfx/Color.hpp>
#include <texts/TextKeysAndLanguages.hpp>

ConsoleContainerBase::ConsoleContainerBase()
{
    setWidth(800);
    setHeight(173);
    background.setPosition(27, 59, 747, 114);
    background.setColor(touchgfx::Color::getColorFromRGB(64, 72, 115));

    backgroundLeft.setPosition(27, 49, 20, 20);
    backgroundLeft.setCenter(10, 10);
    backgroundLeft.setRadius(10);
    backgroundLeft.setLineWidth(0);
    backgroundLeft.setArc(0, 360);
    backgroundLeftPainter.setColor(touchgfx::Color::getColorFromRGB(64, 72, 115));
    backgroundLeft.setPainter(backgroundLeftPainter);

    backgroundRight.setPosition(754, 49, 20, 20);
    backgroundRight.setCenter(10, 10);
    backgroundRight.setRadius(10);
    backgroundRight.setLineWidth(0);
    backgroundRight.setArc(0, 360);
    backgroundRightPainter.setColor(touchgfx::Color::getColorFromRGB(64, 72, 115));
    backgroundRight.setPainter(backgroundRightPainter);

    backgroundTop.setPosition(37, 49, 727, 20);
    backgroundTop.setColor(touchgfx::Color::getColorFromRGB(64, 72, 115));

    consoleTitle.setXY(27, 25);
    consoleTitle.setColor(touchgfx::Color::getColorFromRGB(111, 121, 174));
    consoleTitle.setLinespacing(0);
    consoleTitle.setTypedText(touchgfx::TypedText(T___SINGLEUSE_77SV));

    line1.setPosition(37, 59, 727, 8);
    line1.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line1.setLinespacing(0);
    line1Buffer[0] = 0;
    line1.setWildcard(line1Buffer);
    line1.setTypedText(touchgfx::TypedText(T___SINGLEUSE_P3TH));

    line2.setPosition(37, 68, 727, 8);
    line2.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line2.setLinespacing(0);
    line2Buffer[0] = 0;
    line2.setWildcard(line2Buffer);
    line2.setTypedText(touchgfx::TypedText(T___SINGLEUSE_9WFZ));

    line3.setPosition(37, 77, 727, 8);
    line3.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line3.setLinespacing(0);
    line3Buffer[0] = 0;
    line3.setWildcard(line3Buffer);
    line3.setTypedText(touchgfx::TypedText(T___SINGLEUSE_H6NY));

    line4.setPosition(37, 86, 727, 8);
    line4.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line4.setLinespacing(0);
    line4Buffer[0] = 0;
    line4.setWildcard(line4Buffer);
    line4.setTypedText(touchgfx::TypedText(T___SINGLEUSE_WKBQ));

    line5.setPosition(37, 95, 727, 8);
    line5.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line5.setLinespacing(0);
    line5Buffer[0] = 0;
    line5.setWildcard(line5Buffer);
    line5.setTypedText(touchgfx::TypedText(T___SINGLEUSE_EX5Q));

    line6.setPosition(37, 104, 727, 8);
    line6.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line6.setLinespacing(0);
    line6Buffer[0] = 0;
    line6.setWildcard(line6Buffer);
    line6.setTypedText(touchgfx::TypedText(T___SINGLEUSE_0KBR));

    line7.setPosition(37, 113, 727, 8);
    line7.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line7.setLinespacing(0);
    line7Buffer[0] = 0;
    line7.setWildcard(line7Buffer);
    line7.setTypedText(touchgfx::TypedText(T___SINGLEUSE_1EOB));

    line8.setPosition(37, 122, 727, 8);
    line8.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line8.setLinespacing(0);
    line8Buffer[0] = 0;
    line8.setWildcard(line8Buffer);
    line8.setTypedText(touchgfx::TypedText(T___SINGLEUSE_9UB8));

    line9.setPosition(37, 131, 727, 8);
    line9.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line9.setLinespacing(0);
    line9Buffer[0] = 0;
    line9.setWildcard(line9Buffer);
    line9.setTypedText(touchgfx::TypedText(T___SINGLEUSE_R2FF));

    line10.setPosition(37, 140, 727, 8);
    line10.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line10.setLinespacing(0);
    line10Buffer[0] = 0;
    line10.setWildcard(line10Buffer);
    line10.setTypedText(touchgfx::TypedText(T___SINGLEUSE_1CKO));

    line11.setPosition(37, 149, 727, 8);
    line11.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line11.setLinespacing(0);
    line11Buffer[0] = 0;
    line11.setWildcard(line11Buffer);
    line11.setTypedText(touchgfx::TypedText(T___SINGLEUSE_7HOC));

    line12.setPosition(37, 158, 727, 8);
    line12.setColor(touchgfx::Color::getColorFromRGB(154, 171, 196));
    line12.setLinespacing(0);
    line12Buffer[0] = 0;
    line12.setWildcard(line12Buffer);
    line12.setTypedText(touchgfx::TypedText(T___SINGLEUSE_V34E));

    add(background);
    add(backgroundLeft);
    add(backgroundRight);
    add(backgroundTop);
    add(consoleTitle);
    add(line1);
    add(line2);
    add(line3);
    add(line4);
    add(line5);
    add(line6);
    add(line7);
    add(line8);
    add(line9);
    add(line10);
    add(line11);
    add(line12);
}

void ConsoleContainerBase::initialize()
{

}

