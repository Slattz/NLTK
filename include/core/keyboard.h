#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <vector>
#include "CTRFont.hpp"

enum class KeyboardStatus : s8
{
    Abort = -1,
    Loop = 0,
    Close = 1
};

enum InputTypes : u8 //Used to enable and disable different input types
{
    KType_None = 0,
    KType_Letters = 1,
    KType_Numbers = 2,
    KType_Symbols = 4
};

enum KeyboardTab : u8
{
    KTab_Letters = 0,
    KTab_Symbols = 1,
    KTab_ACNLSymbols = 2
};

class Keyboard {
public:
    Keyboard(void);
    Keyboard(u8 InType, u32 MaxSize = 30, bool CanAbort = true);
    Keyboard(u8 InType, u32 MaxSize, bool CanAbort, const std::string &HintText, const std::string &DefaultText = "");
    Keyboard(u32 MaxSize, bool CanAbort, const std::string &HintText, const u32 &DefaultValue);
    ~Keyboard(void);

    void SetHint(const std::string &HintText);
    void SetText(const std::string &DefaultText);
    void SetValue(const u32 &DefaultValue);
    void CanAbort(bool canAbort);
    void SetMaxLength(u32 maxSize);
    void SetInputType(u8 InType);

    int Open(std::string& output);
    int Open(u32& output);

private:
  void SetupCommonText();
  void SetupLetters();
  void SetupSymbols();
  void SetupACNLSymbols();
  void UpdateHID();
  void Update();
  void Draw();
  int _Open(std::string& output);

  bool m_CanAbort;
  bool m_ShiftOn;
  u32 m_MaxSize;
  u32 m_NinSymbolsPage;
  u8 m_InputType;
  u8 m_KeyboardTab;
  KeyboardStatus m_KeyboardStatus;
  Text m_HintText;
  Text m_Text;
  std::vector<Text> m_CommonText;
  std::vector<Text> m_Characters;
  std::vector<Text> m_Symbols;
  std::vector<Text> m_ACNLSymbols;
};



#endif