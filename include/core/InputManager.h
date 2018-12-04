#pragma once
#include "structs.h"
#include "gfx.h"
#include "cursor.h"

typedef struct {
    Point_t Touch;
    Point_t Cursor;
    bool CursorEnabled;
} Input;

class InputManager {
public:
    bool IsActive(Rect_t rect);
    void RefreshInput();

private:
    bool IsActive(Rect_t rect, u32 x, u32 y);

    Input _input;
};