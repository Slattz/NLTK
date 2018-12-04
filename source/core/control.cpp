#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "gfx.h"
#include "control.h"

// TODO: These should definitely be a struct that is handled by a class somewhere.
extern s16 g_CheckX[2];
extern s16 g_CheckY[2];
extern u32 g_key[2];
extern bool g_disabled[2];

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

void Control::Draw() {
    if (Visible) {
        C2D_DrawRectSolid(Location.X, Location.Y, ZPos, Size.Width, Size.Height, BackgroundColor);
    }
}

void Control::SetZPos(float Zpos) {
    ZPos = Zpos;
}

// Returns whether the control is being interacted with currently.
bool Control::IsActive() {
    // TODO: The same class that handles the g_X input stuff should also have a method
    // to pass a rect and check if the input was inside it.
    for (int i = 0; i < 2; i++) {
        if (hidKeysDown() & g_key[i] && g_disabled[i]) {
            return g_CheckX[i] >= Location.X && g_CheckX[i] <= Location.X + Size.Width &&
                   g_CheckY[i] >= Location.Y && g_CheckY[i] <= Location.Y + Size.Height;
        }
    }

    return false;
}