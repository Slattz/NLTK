#include "textbox.h"

TextBox::TextBox() {
	Hint = std::string("Enter some text.");

	Location = { 0, 0 };
	Size = { 0, 0 };

	BackgroundColor = COLOR_WHITE;
	SetTextColor(COLOR_BLACK);
	SetTextSize(0.f, 0.f);
	SetTextPos(0, 0);
	Visible = true;
}

TextBox::TextBox(Point_t location, Size_t size, std::string text, std::string hint, u32 maxLength, SwkbdType type, u32 bgColor, u32 textColor) 
	: myText(text) {

	Hint = hint;
	MaxLength = maxLength;
	Type = type;

	Location = location;
	Size = size;

	BackgroundColor = bgColor;
    SetTextColor(textColor);
    SetTextSize(FontScale, FontScale);
    SetTextPos(Location.X + 2, Location.Y + 1);
	Visible = true;
}

TextBox::TextBox(u32 x, u32 y, u32 width, u32 height, std::string text, std::string hint, u32 maxLength, SwkbdType type, u32 bgColor, u32 textColor)
	: TextBox(Point_t{x, y}, Size_t{width, height}, text, hint, maxLength, type, bgColor, textColor) { }

void TextBox::Draw() {
	if (Visible) {
		C2D_DrawRectSolid(Location.X, Location.Y, 0, Size.Width, Size.Height, BackgroundColor);
		myText.Draw(Location.X + 2, Location.Y + 1);
	}
}

bool TextBox::CheckActivate(u32 x, u32 y) {
	return Visible && (x >= Location.X && x <= Location.X + Size.Width && y >= Location.Y && y < Location.Y + Size.Height);
}

std::string TextBox::Activate() {
	std::string result = getKeyboardInput(myText, Hint.c_str(), MaxLength, Type);
	if (result.compare(std::string(" ")) != 0) {
		myText = result;
	}

	return myText.GetText();
}

void TextBox::SetVisibility(bool visibility) {
    Visible = visibility;
}

void TextBox::SetTextSize(float scaleX, float scaleY) {
    myText.SetSize(scaleX, scaleY);
}

void TextBox::SetTextColor(u32 color) {
    myText.SetColor(color);
}

void TextBox::SetTextPos(float posX, float posY) {
    myText.SetPos(posX, posY);
}