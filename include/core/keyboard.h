#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "common.h"

enum class KeyboardType : u8
{
    Text = 0,
    Numbers = 1
};

class Keyboard {
public:
    Keyboard(void);
    Keyboard(const std::string& HintText, const std::string& DefaultText = "");
    ~Keyboard(void);

    void SetHint(const std::string &HintText);
    void SetText(const std::string &DefaultText);
    void CanAbort(bool canAbort);
    void SetMaxLength(u32 maxLength);
    
    int Open(std::string& output);

private:
    void Draw();

    Text m_HintText;
    std::string m_Text = "";
    Point_t m_cursorPos;
    bool m_CanAbort = true;
    u32 m_MaxLength = UINT32_MAX;
    KeyboardType m_Type;
    bool m_TopDrawn = false;
};



#endif