#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <vector>
#include "CTRFont.hpp"
#include "button.h"
#include "imagebutton.h"
#include "textbutton.h"

/* Current Status of the Keyboard */
enum class KeyboardStatus : s8
{
    Abort = -1,
    Loop = 0,
    Close = 1
};

/* Return Code of the Keyboard */
enum class KeyboardRetCode : s8
{
    NoInputType = -2,
    Abort = -1,
    Success = 0
};

/* Used to enable various input types */
enum class KeyboardInType : u8
{
    None = 0,
    Letters = 1,
    Numbers = 2,
    Symbols = 4,
    ACNLSymbols = 8,
    All = Numbers|Letters|Symbols|ACNLSymbols
};

/* Used to determine various keyboard tabs */
enum class KeyboardTab : u8
{
    Letters = 0,
    Symbols = 1,
    ACNLSymbols = 2
};

KeyboardTab& operator++(KeyboardTab& tab);
KeyboardTab& operator--(KeyboardTab& tab);

inline u8 operator|(KeyboardInType val, KeyboardInType intype) {return static_cast<u8>(val) | static_cast<u8>(intype);}
inline u8 operator|(u8 val, KeyboardInType intype) {return val|static_cast<u8>(intype);}
inline u8 operator&(KeyboardInType val, KeyboardInType intype) {return static_cast<u8>(val) & static_cast<u8>(intype);}
inline u8 operator&(u8 val, KeyboardInType intype) {return val & static_cast<u8>(intype);}
inline bool operator==(u8 val, KeyboardInType intype) {return val == static_cast<u8>(intype);}

class Keyboard {
public:
    static Keyboard* Instance(void);
    
    KeyboardRetCode Open(std::string &output, u8 InType, u32 MaxSize, const std::string &DefaultText = "", const std::string &HintText = "", bool CanAbort = true);
    KeyboardRetCode Open(std::string &output, KeyboardInType InType, u32 MaxSize, const std::string &DefaultText = "", const std::string &HintText = "", bool CanAbort = true);
    KeyboardRetCode Open(u32 &output, u32 MaxVal, const u32 &DefaultValue = 0, const std::string &HintText = "", bool CanAbort = true);
    KeyboardRetCode Open(u16 &output, u16 MaxVal, const u16 &DefaultValue = 0, const std::string &HintText = "", bool CanAbort = true);
    KeyboardRetCode Open(u8 &output, u8 MaxVal, const u8 &DefaultValue = 0, const std::string &HintText = "", bool CanAbort = true);

  private:
    Keyboard(void);
    ~Keyboard(void);
    
    void Setup();
    void SetupDefaults();
    void SetupLetters();
    void SetupSymbols();
    void SetupACNLSymbols();

    void UpdateHID();
    void Update();
    void Draw();
    KeyboardRetCode _Open(std::string &output);

    static Keyboard* m_Instance;

    bool m_CanAbort;
    bool m_ShiftOn;
    u32 m_MaxSize;
    u32 m_StringSize;
    u32 m_NinSymbolsPage;
    u8 m_InputType;
    KeyboardTab m_KeyboardTab;
    KeyboardStatus m_KeyboardStatus;
    Text m_HintText;
    Text m_Text;
    std::vector<Text> m_Characters;
    std::vector<Text> m_Symbols;
    std::vector<Text> m_ACNLSymbols;
};



#endif