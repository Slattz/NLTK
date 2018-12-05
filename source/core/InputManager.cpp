#include "InputManager.h"

InputManager* InputManager::m_pInputManager = nullptr;

InputManager::InputManager(void) {
    _input.Cursor.X = 160;
    _input.Cursor.Y = 120;
    _input.CursorEnabled = false;
    _input.CursorHeld = false;
}

InputManager* InputManager::Instance() {
    if (!m_pInputManager) {
        m_pInputManager = new InputManager;
    }

    return m_pInputManager;
}

bool InputManager::IsButtonDown(int button) {
    return this->_input.KeysDown & button;
}

bool InputManager::IsButtonHeld(int button) {
    return this->_input.KeysHeld & button;
}

bool InputManager::IsButtonActive(int button) {
    return this->IsButtonDown(button) || this->IsButtonHeld(button);
}

void InputManager::RefreshInput() {
    touchPosition touch;

    hidScanInput();
    hidTouchRead(&touch);

    // Update input
    this->_input.KeysDown = hidKeysDown();
    this->_input.KeysHeld = hidKeysHeld();
    this->_input.Touch.X = touch.px;
    this->_input.Touch.Y = touch.py;

    if (this->IsButtonActive(KEY_TOUCH) && !this->_input.CursorHeld) {
        this->_input.CursorEnabled = false;
        return;
    }

    if (this->IsButtonActive(KEY_A) && _input.CursorEnabled) {
        this->_input.CursorHeld = true;
        return;
    }
    else _input.CursorHeld = false;

    if (this->IsButtonActive(KEY_CPAD)) // C-Pad
    {
        if (this->IsButtonActive(KEY_CPAD_RIGHT))
            this->_input.Cursor.X += 3;

        if (this->IsButtonActive(KEY_CPAD_LEFT))
            this->_input.Cursor.X -= 3;

        if (this->IsButtonActive(KEY_CPAD_UP))
            this->_input.Cursor.Y -= 3;

        if (this->IsButtonActive(KEY_CPAD_DOWN))
            this->_input.Cursor.Y += 3;

        this->_input.CursorEnabled = true;
    }
    else if (this->IsButtonActive(KEY_CSTICK)) // C-Stick
    {
        if (this->IsButtonActive(KEY_CSTICK_RIGHT))
            this->_input.Cursor.X += 4;

        if (this->IsButtonActive(KEY_CSTICK_LEFT))
            this->_input.Cursor.X -= 4;

        if (this->IsButtonActive(KEY_CSTICK_UP))
            this->_input.Cursor.Y -= 4;

        if (this->IsButtonActive(KEY_CSTICK_DOWN))
            this->_input.Cursor.Y += 4;

        this->_input.CursorEnabled = true;
    }

    if (this->_input.Cursor.Y < 5)
        this->_input.Cursor.Y = 235;

    if (this->_input.Cursor.Y > 235)
        this->_input.Cursor.Y = 5;

    if (this->_input.Cursor.X < 5)
        this->_input.Cursor.X = 315;

    if (this->_input.Cursor.X > 315)
        this->_input.Cursor.X = 5;
}

void InputManager::DrawCursor(void) {
    C2D_SceneBegin(bottom);

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
        this->IsButtonActive(KEY_TOUCH) && this->IsActive(rect, this->_input.Touch.X, this->_input.Touch.Y);
}

bool InputManager::IsActive(Rect_t rect, std::initializer_list<u32> keys) {
    if (this->IsActive(rect)) {
        for (auto button : keys) {
            if (!this->IsButtonActive(button)) {
                return false;
            }
        }

        return true;
    }

    return false;
}