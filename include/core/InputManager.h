#pragma once
#include "structs.h"
#include "gfx.h"

enum EXTRA_HID
{
    KEY_CPAD = KEY_CPAD_DOWN | KEY_CPAD_UP | KEY_CPAD_LEFT | KEY_CPAD_RIGHT,
    KEY_CSTICK = KEY_CSTICK_DOWN | KEY_CSTICK_UP | KEY_CSTICK_LEFT | KEY_CSTICK_RIGHT
};

typedef struct {
    Point_t Touch;
    Point_t Cursor;
    bool CursorEnabled;
    bool CursorHeld;
} Input;

class InputManager {
public:
    InputManager(void);
    bool IsActive(Rect_t rect);
    void RefreshInput();
    void DrawCursor(void);

private:
    bool IsActive(Rect_t rect, u32 x, u32 y);

    Input _input;
};