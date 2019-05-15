#include "label.h"

Label::Label(void) : Control() { }

Label::Label(Point_t location, Size_t size, u32 bgColor, u32 textColor, std::string text)
    : Control(location, size, bgColor), myText(text) {

    SetTextColor(textColor);
    SetTextSize(FontScale, FontScale);
    SetTextPos(Location.X + 2, Location.Y);
    Visible = true;
};

Label::Label(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 textColor, std::string text)
    : Label(Point_t{ x, y }, Size_t{ width, height }, bgColor, textColor, text) { }

void Label::Draw(void) {
    if (Visible) {
        Control::Draw();
        myText.Draw();
    }
}

void Label::SetVisibility(bool visibility) {
    Visible = visibility;
}

void Label::SetTextSize(float scaleX, float scaleY) {
    myText.SetSize(scaleX, scaleY);
}

void Label::SetTextColor(u32 color) {
    myText.SetColor(color);
}

void Label::SetTextPos(float posX, float posY) {
    myText.SetPos(posX, posY);
}

void Label::CenterInBounds(float left, float top, float width, float height) {
    myText.CenterInBounds(left, top, width, height);
}
