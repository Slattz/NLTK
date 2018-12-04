#pragma once

#include "structs.h"
#include "InputManager.h"

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
    virtual bool IsActive();
    void SetZPos(float Zpos);
    Rect_t GetActiveArea(void);

    Point_t Location;
    Size_t Size;

    u32 BackgroundColor;
    bool Visible;
    float ZPos = 0.f;

protected:
    Rect_t ActiveArea;
};