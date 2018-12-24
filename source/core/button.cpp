#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "gfx.h"
#include "button.h"

Button::Button() 
    : Control({0, 0}, {0, 0}, COLOR_WHITE) {
    Visible = true;
}

Button::Button(Point_t location, Size_t size, u32 bgColor, u32 activatorKeys) 
    : Control(location, size, bgColor) {

    m_activatorKeys = activatorKeys;
    Visible = true;
}

Button::Button(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 activatorKeys)
    : Button(Point_t{ x, y }, Size_t{ width, height }, bgColor, activatorKeys) { }

void Button::Draw(void) {
    if (Visible) {
        if (m_active) {
            C2D_DrawRectSolid(Location.X, Location.Y, ZPos, Size.Width, Size.Height, BackgroundColor - 0x00101010); // NOTE: If we have a black button this won't work
        }
        else {
            C2D_DrawRectSolid(Location.X, Location.Y, ZPos, Size.Width, Size.Height, BackgroundColor);
        }
    }
}

void Button::SetCallback(void(*callback)(Button *sender)) {
    m_callback = callback;
}

bool Button::CheckActivate(u32 x, u32 y) {
    return Visible && (x >= Location.X && x <= Location.X + Size.Width && y >= Location.Y && y < Location.Y + Size.Height) && (InputManager::Instance()->IsButtonDown(m_activatorKeys));
}

bool Button::SetActive(bool active) {
    m_active = active;
    return m_active;
}

void Button::Activate(void) {
    if (m_callback != nullptr) {
        m_callback(this);
    }
}

void Button::SetActivatorKeys(u32 activatorKeys) {
    m_activatorKeys = activatorKeys;
}

bool Button::IsActive(void) {
    return InputManager::Instance()->IsActive(this->ActiveArea, { this->m_activatorKeys });
}

bool Button::IsActive(u32 activatorKeys) {
    return InputManager::Instance()->IsActive(this->ActiveArea, { activatorKeys });
}

bool Button::IsDown(void) {
    return InputManager::Instance()->IsDown(this->ActiveArea, { this->m_activatorKeys });
}

bool Button::IsHeld(void) {
    return InputManager::Instance()->IsHeld(this->ActiveArea, { this->m_activatorKeys });
}