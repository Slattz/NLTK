#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "common.h"
enum class KeyboardStatus : s8
{
    Abort = -1,
    Loop = 0,
    Close = 1
};

enum class InputTypes : u8 //Used to enable and disable different input types
{
    None = 0,
    Text = 1,
    Numbers = 2,
    Symbols = 4
};

enum class KeyboardTab : u8
{
    Text = 0,
    Symbols = 1,
    ACNLSymbols = 2
};

class Keyboard {
public:
    Keyboard(void);
    Keyboard(InputTypes InType, u32 MaxSize = 30, bool CanAbort = true);
    Keyboard(InputTypes InType, u32 MaxSize, bool CanAbort, const std::string &HintText, const std::string &DefaultText = "");
    Keyboard(u32 MaxSize, bool CanAbort, const std::string &HintText, const u32 &DefaultValue);
    ~Keyboard(void);

    void SetHint(const std::string &HintText);
    void SetText(const std::string &DefaultText);
    void SetValue(const u32 &DefaultValue);
    void CanAbort(bool canAbort);
    void SetMaxLength(u32 maxSize);
    void SetInputType(InputTypes InType);

    int Open(std::string& output);
    int Open(u32& output);

private:
    void SetupLetters();
    void SetupSymbols();
    void SetupACNLSymbols();
    void SetupComm();
    void UpdateHID();
    void Update();
    void Draw();
    int _Open(std::string& output);

    u32 m_ButtonIndex;
    KeyboardTab m_KeyboardTab;
    u32 m_MaxSize;
    bool m_CanAbort;
    bool m_ShiftOn;
    InputTypes m_InputType;
    KeyboardStatus m_KeyboardStatus;
    Text m_HintText;
    Text m_Text;
    std::vector<Text> m_Characters;
    std::vector<Text> m_Symbols;
    std::vector<Text> m_ACNLSymbols;
};



#endif