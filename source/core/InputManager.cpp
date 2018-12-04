#include "InputManager.h"

InputManager::InputManager(void) {
    _input.Cursor.X = 160;
    _input.Cursor.Y = 120;
    _input.CursorEnabled = false;
    _input.CursorHeld = false;
}

void InputManager::RefreshInput() {
    touchPosition touch;
    u32 kHeld;

    hidScanInput();
    hidTouchRead(&touch);
    kHeld = hidKeysHeld();

    _input.Touch.X = touch.px;
    _input.Touch.Y = touch.py;

    if (kHeld & KEY_TOUCH && !_input.CursorHeld) {
        _input.CursorEnabled = false;
        return;
    }

    
    if (kHeld & KEY_A && _input.CursorEnabled) {
        _input.CursorHeld = true;
        return;
    }

    else _input.CursorHeld = false;

    //CPad
    if (kHeld & KEY_CPAD)
    {
        if (kHeld & KEY_CPAD_RIGHT)
            _input.Cursor.X += 2;

        if (kHeld & KEY_CPAD_LEFT)
            _input.Cursor.X -= 2;

        if (kHeld & KEY_CPAD_UP)
            _input.Cursor.Y -= 2;

        if (kHeld & KEY_CPAD_DOWN)
            _input.Cursor.Y += 2;

        _input.CursorEnabled = true;
    }

    //C-Stick
    else if (kHeld & KEY_CSTICK)
    {
        if (hidKeysHeld() & KEY_CSTICK_RIGHT)
            _input.Cursor.X += 3;

        if (hidKeysHeld() & KEY_CSTICK_LEFT)
            _input.Cursor.X -= 3;

        if (hidKeysHeld() & KEY_CSTICK_UP)
            _input.Cursor.Y -= 3;

        if (hidKeysHeld() & KEY_CSTICK_DOWN)
            _input.Cursor.Y += 3;

        _input.CursorEnabled = true;
    }

    if (_input.Cursor.Y < 5)
        _input.Cursor.Y = 235;

    if (_input.Cursor.Y > 235)
        _input.Cursor.Y = 5;

    if (_input.Cursor.X < 5)
        _input.Cursor.X = 315;

    if (_input.Cursor.X > 315)
        _input.Cursor.X = 5;
}

void InputManager::DrawCursor(void) {
    C2D_SceneBegin(bottom);
    RefreshInput();

    if (_input.CursorEnabled)
    {
        if (_input.CursorHeld)
            DrawSprite(Common_ss, CURSOR_SELECT, _input.Cursor.X, _input.Cursor.Y, nullptr, 1.0f, 1.0f, 1.0f);

        else
            DrawSprite(Common_ss, CURSOR_POINT, _input.Cursor.X, _input.Cursor.Y, nullptr, 1.0f, 1.0f, 1.0f);
    }

    C2D_Flush();
}

bool InputManager::IsActive(Rect_t rect, u32 x, u32 y) {
    return x >= rect.TopLeft.X && x <= rect.BottomRight.X &&
           y >= rect.TopLeft.Y && y <= rect.BottomRight.Y;
}

bool InputManager::IsActive(Rect_t rect) {
    return _input.CursorEnabled && _input.CursorHeld ? this->IsActive(rect, this->_input.Cursor.X, this->_input.Cursor.Y) :
        this->IsActive(rect, this->_input.Touch.X, this->_input.Touch.Y);
}