#pragma once

#include "control.h"

class Button : public Control {
public:
    Button(void);
    Button(Point_t location, Size_t size, u32 bgColor, u32 activatorKeys);
    Button(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 activatorKeys);

    virtual void Draw(void);
    void SetCallback(void (*callback)(Button *sender)); // Callback functions can be passed like so: Button::SetCallback(func) OR Button::SetCallback(&func)
    bool CheckActivate(u32 x, u32 y);
    bool SetActive(bool active);
    void Activate(void);
    void SetActivatorKeys(u32 activatorKeys);

    bool m_active = false;

protected:
    void (*m_callback)(Button *sender);
    u32 m_activatorKeys = KEY_TOUCH | KEY_A;
};