#pragma once

#include "control.h"

class Label : public Control {
public:
	Label(void);
	Label(Point_t location, Size_t size, u32 bgColor, u32 textColor, std::string text);
	Label(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 textColor, std::string text);

	void Draw(void);

	std::string Text;
	float FontScale = 0.5;
	u32 TextColor;
};