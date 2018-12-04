#pragma once

#include "structs.h"

/*
    Control Class
        => The base class for all controls.
*/
class Control {
public:
    Control(void);
    Control(Point_t location, Size_t size, u32 backgroundColor);
    Control(u32 x, u32 y, u32 width, u32 height, u32 backgroundColor);

    virtual void Draw();
    void SetZPos(float Zpos);
    bool IsActive();
    Rect_t GetActiveArea(void);

    Point_t Location;
    Size_t Size;

    u32 BackgroundColor;
    bool Visible;
    float ZPos = 0.f;

private:
    Rect_t ActiveArea;
};