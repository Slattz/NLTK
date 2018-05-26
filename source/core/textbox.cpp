#include "textbox.h"

TextBox::TextBox() {
	Text = std::string(" ");
	Hint = std::string("Enter some text.");

	Location = { 0, 0 };
	Size = { 0, 0 };

	BackgroundColor = COLOR_WHITE;
	TextColor = COLOR_BLACK;
	Visible = true;
}

TextBox::TextBox(Point_t location, Size_t size, std::string text, std::string hint, u32 maxLength, SwkbdType type, u32 bgColor, u32 textColor) {
	Text = text;
	Hint = hint;
	MaxLength = maxLength;
	Type = type;

	Location = location;
	Size = size;

	BackgroundColor = bgColor;
	TextColor = textColor;
	Visible = true;
}

TextBox::TextBox(u32 x, u32 y, u32 width, u32 height, std::string text, std::string hint, u32 maxLength, SwkbdType type, u32 bgColor, u32 textColor)
	: TextBox(Point_t{x, y}, Size_t{width, height}, text, hint, maxLength, type, bgColor, textColor) { }

void TextBox::Draw() {
	if (Visible) {
		C2D_DrawRectSolid(Location.X, Location.Y, 0, Size.Width, Size.Height, BackgroundColor);
		DrawText(Location.X + 2, Location.Y, 0.5f, 0.5f, TextColor, Text.c_str());
	}
}

bool TextBox::CheckActivate(u32 x, u32 y) {
	return Visible && (x >= Location.X && x <= Location.X + Size.Width && y >= Location.Y && y < Location.Y + Size.Height);
}

std::string TextBox::Activate() {
	std::string result = getKeyboardInput(Text.c_str(), Hint.c_str(), MaxLength, Type);
	if (result.compare(std::string(" ")) != 0) {
		Text = result;
	}

	return Text;
}