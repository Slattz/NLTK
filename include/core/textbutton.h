#pragma once

#include "button.h"
#include "CTRFont.hpp"

class TextButton : public Button {
public:
    TextButton(void);
    TextButton(Point_t location, Size_t size, u32 bgColor, u32 activatorKeys, u32 textColor, std::string text);
    TextButton(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 activatorKeys, u32 textColor, std::string text);
    TextButton(u32 x, u32 y, u32 bgColor, u32 activatorKeys, u32 textColor, std::string text);

    void Draw(void);
    
    void SetVisibility(bool visibility);
    void SetTextSize(float scaleX, float scaleY);
    void SetTextColor(u32 color);
    void SetTextPos(float posX, float posY);
    void CentreText(void);

    Text myText;
    float FontScale = 1.f;
};