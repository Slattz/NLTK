#include "InputManager.h"

// TODO: This should definitely be implemented into this class!
extern Cursor g_cursor;

void InputManager::RefreshInput() {
    touchPosition touch;
    hidTouchRead(&touch);

    _input.Touch.X = touch.px;
    _input.Touch.Y = touch.py;

    _input.Cursor.X = g_cursor.m_LocX;
    _input.Cursor.Y = g_cursor.m_LocY;

    _input.CursorEnabled = g_cursor.m_Visible;
}

bool InputManager::IsActive(Rect_t rect, u32 x, u32 y) {
    return x >= rect.TopLeft.X && x <= rect.BottomRight.X &&
           y >= rect.TopLeft.Y && y <= rect.BottomRight.Y;
}

bool InputManager::IsActive(Rect_t rect) {
    return _input.CursorEnabled ? this->IsActive(rect, this->_input.Cursor.X, this->_input.Cursor.Y) :
        this->IsActive(rect, this->_input.Touch.X, this->_input.Touch.Y);
}