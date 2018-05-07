#include "textbutton.h"

TextButton::TextButton(void) : Button() { }

TextButton::TextButton(Point_t location, Size_t size, u32 bgColor, u32 activatorKeys, u32 textColor, std::string text)
	: Button(location, size, bgColor, activatorKeys) {
	
	Text = text;
	TextColor = textColor;
};

TextButton::TextButton(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 activatorKeys, u32 textColor, std::string text)
	: TextButton(Point_t{ x, y }, Size_t{ width, height }, bgColor, activatorKeys, textColor, text) { }

void TextButton::Draw(void) {
	Button::Draw();
	pp2d_draw_text_wrap(Location.X + 2, Location.Y + 2, FontScale, FontScale, TextColor, Size.Width - 4, Text.c_str());
}