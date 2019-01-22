#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "gfx.h"
#include "control.h"

Control::Control() {
    Location = { 0, 0 };
    Size = { 0, 0 };

    BackgroundColor = COLOR_WHITE;
    Visible = true;
}

Control::Control(Point_t location, Size_t size, u32 bgColor) {
    Location = location;
    Size = size;
    ActiveArea = Rect_t{ location, Point_t { location.X + size.Width, location.Y + size.Height } };

    BackgroundColor = bgColor;
    Visible = true;
}

Control::Control(u32 x, u32 y, u32 width, u32 height, u32 bgColor)
    : Control(Point_t{ x, y }, Size_t{ width, height }, bgColor) { }

// Empty destructor so that inheriting classes don't implement undefined behavior. We don't allocate anything dynamically.
Control::~Control() { }

void Control::Draw() {
    if (Visible) {
        C2D_DrawRectSolid(Location.X, Location.Y, ZPos, Size.Width, Size.Height, BackgroundColor);
    }
}

void Control::SetZPos(float Zpos) {
    ZPos = Zpos;
}

void Control::SetSize(Size_t size) {
    Size = size;
    ActiveArea = Rect_t{Location, Point_t{Location.X + size.Width, Location.Y + size.Height}};
}

Rect_t Control::GetActiveArea(void) {
    return ActiveArea;
}

bool Control::IsActive(void) {
    return InputManager::Instance()->IsActive(this->ActiveArea);
}