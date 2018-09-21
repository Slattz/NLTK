#pragma once

#include "control.h"
#include "CTRFont.hpp"

class Label : public Control {
public:
    Label(void);
    Label(Point_t location, Size_t size, u32 bgColor, u32 textColor, std::string text);
    Label(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 textColor, std::string text);

    void Draw(void);
    
    void SetVisibility(bool visibility);
    void SetTextSize(float scaleX, float scaleY);
    void SetTextColor(u32 color);
    void SetTextPos(float posX, float posY);

    Text myText;
    float FontScale = 0.5;
};