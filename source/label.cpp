#include "label.h"

Label::Label(void) : Control() { }

Label::Label(Point_t location, Size_t size, u32 bgColor, u32 textColor, std::string text)
	: Control(location, size, bgColor) {

	Text = text;
	TextColor = textColor;
	Visible = true;
};

Label::Label(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 textColor, std::string text)
	: Label(Point_t{ x, y }, Size_t{ width, height }, bgColor, textColor, text) { }

void Label::Draw(void) {
	if (Visible) {
		Control::Draw();
		pp2d_draw_text_wrap(Location.X + 2, Location.Y, FontScale, FontScale, TextColor, Size.Width - 4, Text.c_str());
	}
}