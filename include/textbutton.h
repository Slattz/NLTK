#pragma once

#include "button.h"

class TextButton : public Button {
public:
	TextButton(void);
	TextButton(Point_t location, Size_t size, u32 bgColor, u32 activatorKeys, u32 textColor, std::string text);
	TextButton(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 activatorKeys, u32 textColor, std::string text);

	void Draw(void);

	float FontScale = 1;
	std::string Text;
	u32 TextColor;
};