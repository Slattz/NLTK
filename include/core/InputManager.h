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
    u32 KeysDown;
    u32 KeysHeld;
    bool CursorEnabled;
    bool CursorHeld;
    bool CursorDown;
} Input;

class InputManager {
public:
    static InputManager* Instance();

    bool IsButtonDown(const int& button);
    bool IsButtonHeld(const int& button);
    bool IsButtonActive(const int& button);
    bool IsDown(const Rect_t& rect);
    bool IsDown(const Rect_t& rect, std::initializer_list<u32> keys);
    bool IsHeld(const Rect_t& rect);
    bool IsHeld(const Rect_t& rect, std::initializer_list<u32> keys);
    bool IsActive(const Rect_t& rect);
    bool IsActive(const Rect_t& rect, std::initializer_list<u32> keys);
    void RefreshInput(void);
    void DrawCursor(void);

private:
    bool IsActive(const Rect_t& rect, u32 x, u32 y);

    InputManager(void);
    InputManager(InputManager const&) {};
    InputManager& operator=(InputManager const&) { return *InputManager::Instance(); }; // Is this right?
    static InputManager* m_pInputManager;

    Input _input;
};