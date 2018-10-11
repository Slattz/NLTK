#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <vector>
#include "CTRFont.hpp"
#include "button.h"
#include "imagebutton.h"
#include "textbutton.h"

enum class KeyboardStatus : s8
{
    Abort = -1,
    Loop = 0,
    Close = 1
};

enum class KeyboardInType : u8 //Used to enable and disable different input types
{
    None = 0,
    Letters = 1,
    Numbers = 2,
    Symbols = 4,
    ACNLSymbols = 8
};

enum class KeyboardTab : u8
{
    Letters = 0,
    Symbols = 1,
    ACNLSymbols = 2
};

KeyboardTab& operator++(KeyboardTab& tab);
KeyboardTab& operator--(KeyboardTab& tab);
u8 operator|(KeyboardInType val, KeyboardInType orval);
u8 operator|(u8 val, KeyboardInType orval);

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
  void SetupCommon();
  void SetupLetters();
  void SetupSymbols();
  void SetupACNLSymbols();
  void UpdateHID();
  void Update();
  void Draw();
  int _Open(std::string& output);

  bool m_CanAbort = true;
  bool m_ShiftOn = false;
  u32 m_MaxSize;
  u32 m_NinSymbolsPage = 0;
  u8  m_InputType;
  KeyboardTab m_KeyboardTab = KeyboardTab::Letters;
  KeyboardStatus m_KeyboardStatus = KeyboardStatus::Loop;
  Text m_HintText;
  Text m_Text;
  std::vector<Text> m_Characters;
  std::vector<Text> m_Symbols;
  std::vector<Text> m_ACNLSymbols;

  Button*      SpaceButton;
  ImageButton* BackButton;
  ImageButton* EnterButton;
  ImageButton* ConfirmButton;
  ImageButton* CancelButton;
  TextButton*  LetterTab;
  TextButton*  SymTab;
  TextButton*  NinSymTab;
  TextButton*  Comma;
  TextButton*  FStop;
  TextButton*  UpBtn;
  TextButton*  DownBtn;
  Text         Page;
  Text         PageNum;
};



#endif