#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "gfx.h"
#include "utils.h"
#include "keyboard.h"
#include "InputManager.h"

#define KEYS_PER_ROW 10
#define KEYBOARD_ROWS 4
#define KEYS_AMOUNT 36

static const u32 SWKBD_BG = C2D_Color32(0x21, 0x8B, 0x2B, 0xFF);
static const u32 SWKBD_BAR = C2D_Color32(0x14, 0x56, 0x1A, 0xFF);
static const u32 SWKBD_TAB_CLR = C2D_Color32(0x35, 0xCC, 0x1A, 0xFF);
static const u32 SWKBD_CHAR_BG = C2D_Color32(0x19, 0x68, 0x1F, 0xFF);
static const u32 SWKBD_INFILTER = C2D_Color32(119, 119, 119, 170);
static constexpr Rect_t BG_RECT = {{37, 36}, {282, 189}};

static constexpr u8 kbd_lyt[] = {0,0,1,3};
static constexpr char Letters[] = "1234567890qwertyuiopasdfghjklzxcvbnm";
static constexpr wchar_t Symbols[] = L"@#$_&-+=!?*\"\':;()<>%&[]{}▲▼◀▶£€|\\/♂♀"; //Needs to be wchar_t as £ and € are utf-16 :/
static constexpr char16_t NinSymbols[] = { //Laid out in same layout as characters per keyboard row: 10,10,9,7
    0xE000, 0xE001, 0xE002, 0xE003, 0xE004, 0xE005, 0xE006, 0xE077, 0xE008, 0xE009, //10
    0xE070, 0xE06F, 0xE06C, 0xE00C, 0xE00D, 0xE00E, 0xE00F, 0xE010, 0xE011, 0xE012, //10
    0xE013, 0xE03C, 0xE03B, 0xE03D, 0xE072, 0xE019, 0xE01A, 0xE01B, 0xE01C,         //9
    0xE014, 0xE01D, 0xE01E, 0xE020, 0xE021, 0xE022, 0xE023, //End of page 1         //7

    0xE024, 0xE025, 0xE026, 0xE027, 0xE028, 0xE029, 0xE02A, 0xE02B, 0xE02C, 0xE02D, //10
    0xE02E, 0xE02F, 0xE030, 0xE031, 0xE032, 0xE033, 0xE034, 0xE035, 0xE036, 0xE037, //10
    0xE038, 0xE039, 0xE079, 0xE07A, 0xE07B, 0xE07C, 0xE03E, 0xE03F, 0xE040,         //9
    0xE078, 0xE041, 0xE042, 0xE043, 0xE044, 0xE045, 0xE046, //End of page 2         //7

    0xE047, 0xE048, 0xE049, 0xE04A, 0xE04B, 0xE04C, 0xE04D, 0xE04E, 0xE04F, 0xE050, //10
    0xE051, 0xE052, 0xE053, 0xE054, 0xE055, 0xE056, 0xE057, 0xE058, 0xE059, 0xE05A, //10
    0xE05B, 0xE05C, 0xE05D, 0xE05E, 0xE05F, 0xE060, 0xE061, 0xE062, 0xE063,         //9
    0xE064, 0xE065, 0xE067, 0xE073, 0xE074, 0xE076, 0xE075  //End of page 3         //7
};

Button *SpaceButton;
ImageButton *BackButton;
ImageButton *EnterButton;
ImageButton *ConfirmButton;
ImageButton *CancelButton;
TextButton *LetterTab;
TextButton *SymTab;
TextButton *NinSymTab;
TextButton *Comma;
TextButton *FStop;
TextButton *UpBtn;
TextButton *DownBtn;
Text PageText;
Text PageNumText;

Keyboard* Keyboard::m_Instance = nullptr;

Keyboard::Keyboard(void) {
    Setup();
    m_HintText = Text(COLOR_DARK_GREY, "", 1.f, 1.f);
    m_Text = Text(COLOR_WHITE, "", 1.f, 1.f);
}

Keyboard::~Keyboard(void) {
    m_Characters.clear();
    m_Symbols.clear();
    m_ACNLSymbols.clear();
    m_Characters.shrink_to_fit();
    m_Symbols.shrink_to_fit();
    m_ACNLSymbols.shrink_to_fit();
    m_Instance = nullptr;
}

Keyboard* Keyboard::Instance(void) {
    if (m_Instance == nullptr) {
        m_Instance = new Keyboard;
    }

    return m_Instance;
}

void Keyboard::SetupDefaults(void) {
    m_KeyboardTab = KeyboardTab::Letters;
    m_KeyboardStatus = KeyboardStatus::Loop;
    m_InputType = 0;
    m_MaxSize = 30;
    m_CanAbort = true;
    m_ShiftOn = false;
    m_NinSymbolsPage = 0;
}

void Keyboard::SetupLetters() {
    u8 CurIndex = 0;
    m_Characters.clear();

    for (u32 i = 0; i < KEYBOARD_ROWS; i++)
    {
        float indent = 0.f;
        if (kbd_lyt[i] == 1) indent = 15.f; //'asdfghjkl' indent
        else if (kbd_lyt[i] == 3) indent = 39.5f; //'zxcvbnm' indent
        u8 KeysPerRow = KEYS_PER_ROW - kbd_lyt[i];

        for (u32 j = 0; j < KeysPerRow; j++)
        {
            char letter = (m_ShiftOn && i > 0) ? (Letters[j+CurIndex]-0x20) : (Letters[j+CurIndex]); //i > 0 check so nums not affected, -0x20 = capital
            Text txt(COLOR_WHITE, std::string(1, letter), 1.0f, 1.0f, 42.f + (25.f * j) + indent, 40.f + (30.f * i));
            m_Characters.push_back(txt);
        }
        CurIndex += KeysPerRow;
    }
}

void Keyboard::SetupSymbols() {
    u8 CurIndex = 0;
    m_Symbols.clear();

    for (u32 i = 0; i < KEYBOARD_ROWS; i++)
    {
        float indent = 0.f;
        if (kbd_lyt[i] == 1) indent = 15.f; //1st indent
        else if (kbd_lyt[i] == 3) indent = 39.5f; //2nd indent
        u8 KeysPerRow = KEYS_PER_ROW - kbd_lyt[i];

        for (u32 j = 0; j < KeysPerRow; j++)
        {
            std::u16string str(reinterpret_cast<const char16_t *>(Symbols+ (j + CurIndex)), 1);
            Text txt(COLOR_WHITE, u16tou8(str), 1.0f, 1.0f, 42.f + (25.f*j) + indent, 40.f + (30.f*i));
            m_Symbols.push_back(txt);
        }
        CurIndex += KeysPerRow;
    }
}

void Keyboard::SetupACNLSymbols() {
    u8 CurIndex = 0;
    m_ACNLSymbols.clear();

    for(u32 page = 0; page < 3; page++)
    {
        for (u32 i = 0; i < KEYBOARD_ROWS; i++)
        {
            float indent = 0.f;
            if (kbd_lyt[i] == 1)
                indent = 15.f; //1st indent
            else if (kbd_lyt[i] == 3)
                indent = 39.5f; //2nd indent
            u8 KeysPerRow = KEYS_PER_ROW - kbd_lyt[i];
            for (u32 j = 0; j < KeysPerRow; j++)
            {
                std::u16string str(reinterpret_cast<const char16_t *>(NinSymbols + (j + CurIndex)), 1);
                Text txt(COLOR_WHITE, u16tou8(str), 1.0f, 1.0f, 42.f + (25.f * j) + indent, 40.f + (30.f * i));
                m_ACNLSymbols.push_back(txt);
            }
            CurIndex += KeysPerRow;
        }
    }
}

void Keyboard::Setup() {
    SetupDefaults();
    SetupLetters();
    SetupSymbols();
    SetupACNLSymbols();
    Comma = new TextButton(83.f, 155.f, 1.f, 1.f, SWKBD_CHAR_BG, KEY_A | KEY_TOUCH, COLOR_WHITE, ",");
    FStop = new TextButton(235.f, 155.f, 1.f, 1.f, SWKBD_CHAR_BG, KEY_A | KEY_TOUCH, COLOR_WHITE, ".");
    LetterTab = new TextButton(0.f, 200.f, 100.f, 40.f, SWKBD_BAR, KEY_A|KEY_TOUCH, COLOR_WHITE, "ABC\n123");
    SymTab = new TextButton(100.f, 200.f, 124.f, 40.f, SWKBD_BAR, KEY_A|KEY_TOUCH, COLOR_WHITE, "Symbols");
    NinSymTab = new TextButton(224.f, 200.f, 98.f, 40.f, SWKBD_BAR, KEY_A|KEY_TOUCH, COLOR_WHITE, "Nintendo\nSymbols");
    UpBtn = new TextButton(293.f, 43.f, 1.f, 1.f, SWKBD_BG, KEY_A|KEY_TOUCH, COLOR_GREY, "▲");
    DownBtn = new TextButton(293.f, 93.f, 1.f, 1.f, SWKBD_BG, KEY_A|KEY_TOUCH, COLOR_GREY, "▼");
    SpaceButton = new Button(100.f, 162.f, 124.f, 20.f, COLOR_GREY, KEY_A|KEY_TOUCH);
    BackButton = new ImageButton(255.f, 127.f, 0, 0, 0, KEY_A|KEY_TOUCH, SWKBD_BACK, Swkbd_ss);
    EnterButton = new ImageButton(256.f, 160.f, 0, 0, 0, KEY_A|KEY_TOUCH, SWKBD_ENTER, Swkbd_ss);
    ConfirmButton = new ImageButton(287.f, 127.f, 0, 0, 0, KEY_A|KEY_TOUCH, SWKBD_CONFIRM, Swkbd_ss);
    CancelButton = new ImageButton(287.f, 160.f, 0, 0, 0, KEY_A|KEY_TOUCH, SWKBD_CANCEL, Swkbd_ss);
    PageText = Text(COLOR_GREY, "Page", 0.6f, 0.6f, 290.f, 65.f);
    PageNumText = Text(COLOR_GREY, "1", 0.8f, 0.8f, 299.f, 80.f);
    Comma->SetTextSize(0.8f, 0.8f);
    FStop->SetTextSize(0.8f, 0.8f);
    LetterTab->SetTextSize(0.6f, 0.6f);
    SymTab->SetTextSize(0.6f, 0.6f);
    NinSymTab->SetTextSize(0.6f, 0.6f);
    UpBtn->SetTextSize(0.8f, 0.8f);
    DownBtn->SetTextSize(0.8f, 0.8f);
    BackButton->SetScale(0.8f);
    EnterButton->SetScale(0.8f);
    ConfirmButton->SetScale(0.8f);
    CancelButton->SetScale(0.8f);
    SpaceButton->SetZPos(0.5f);
    BackButton->SetZPos(0.5f);
    EnterButton->SetZPos(0.5f);
    ConfirmButton->SetZPos(0.5f);
    CancelButton->SetZPos(0.5f);
    LetterTab->CentreText();
    SymTab->CentreText();
    NinSymTab->CentreText();
}

void Keyboard::Draw() {
    draw_base_interface();
    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, SWKBD_BG);
    C2D_DrawRectSolid(0.f, 0.f, 0.f, 320.f, 25.f, SWKBD_BAR);           //Bar for (Hint) Text Background (Top)
    C2D_DrawRectSolid(283.f, 122.f, 0.f, 33.f, 68.f, SWKBD_CHAR_BG);    //Bar for Confirm/Cancel (Right)
    C2D_DrawRectSolid(38.f, 37.f, 0.5f, 245.f, 153.f, SWKBD_CHAR_BG);   //Characters BG
    C2D_DrawRectSolid(99.f, 161.f, 0.5f, 126.f, 22.f, COLOR_DARK_GREY); //Spacebar Outline
    SpaceButton->Draw();                                                //Spacebar
    BackButton->Draw();                                                 //Keyboard Back Icon
    EnterButton->Draw();                                                //Keyboard Enter Icon
    ConfirmButton->Draw();                                              //Keyboard Confirm Icon
    CancelButton->Draw();                                               //Keyboard Cancel Icon
    Comma->Draw();                                                      //Comma
    FStop->Draw();                                                      //Full Stops

    m_HintText.CenterInBounds(2.5f, 2.5f, 317.5f, 22.5f); //Update Hint text position + width
    m_Text.CenterInBounds(2.5f, 2.5f, 317.5f, 22.5f); //Update Text position + width
    if (m_Text.GetText().empty()) //Check if main text contains no characters
        m_HintText.Draw();
    else
        m_Text.Draw();

    LetterTab->SetActive(false);
    SymTab->SetActive(false);
    NinSymTab->SetActive(false);

    switch (m_KeyboardTab) //Draw correct keys per tab
    {
        case KeyboardTab::Letters :
            for (u32 i = 0; i < m_Characters.size(); i++)
                m_Characters[i].Draw();

            if (m_ShiftOn) DrawSprite(Swkbd_ss, SWKBD_SHIFT_ON, 45.f, 128.f, nullptr, 0.8f, 0.8f, 0.5f); //Keyboard Shift Icon
            else  DrawSprite(Swkbd_ss, SWKBD_SHIFT_OFF, 45.f, 128.f, nullptr, 0.8f, 0.8f, 0.5f); //Keyboard Shift Icon
            LetterTab->SetActive(true);                                                         //Highlight selected tab
            if (!(m_InputType & KeyboardInType::Letters)) {//Check if Letters not enabled
                for (u32 i = 1; i < KEYBOARD_ROWS; i++)
                {
                    float indent = 0.f;
                    u8 KeysPerRow = KEYS_PER_ROW - kbd_lyt[i];
                    if (kbd_lyt[i] == 1) indent = 15.f; //'asdfghjkl' indent
                    else if (kbd_lyt[i] == 3) indent = 39.5f; //'zxcvbnm' indent
                    C2D_DrawRectSolid(38.f+indent, 40.f + (30.f * i), 0.5f, (24.5f*KeysPerRow), 30.f, SWKBD_INFILTER);
                }
            }
            if (!(m_InputType & KeyboardInType::Numbers)) { //Check if Numbers not enabled
                C2D_DrawRectSolid(38.f, 37.f, 0.5f, (24.5f*KEYS_PER_ROW), 33.f, SWKBD_INFILTER);
            }
            break;

        case KeyboardTab::Symbols :
            for (u32 i = 0; i < m_Symbols.size(); i++)
                m_Symbols[i].Draw();

            SymTab->SetActive(true); //Highlight selected tab
            break;

        case KeyboardTab::ACNLSymbols :
            for (u32 i = 0; i < m_ACNLSymbols.size()/3; i++) {
                m_ACNLSymbols[i + (m_ACNLSymbols.size()/3 * m_NinSymbolsPage)].Draw(); //Should be 36 is amount of characters in one page
            }

            NinSymTab->SetActive(true);                    //Highlight selected tab
            PageNumText = std::to_string(m_NinSymbolsPage + 1); //Update page number text

            UpBtn->Draw();
            DownBtn->Draw();
            PageText.Draw();
            PageNumText.Draw();
            break;

        default:
            break;
    }
    LetterTab->Draw();
    SymTab->Draw();
    NinSymTab->Draw();

    InputManager::Instance()->DrawCursor();

    C3D_FrameEnd(0);
}
void Keyboard::UpdateHID() {
    InputManager::Instance()->RefreshInput(); //Update cursor info

    if (InputManager::Instance()->IsButtonDown(KEY_DUP) && m_KeyboardTab == KeyboardTab::ACNLSymbols) {

        if (m_NinSymbolsPage == 0)
            m_NinSymbolsPage = 2;

        else m_NinSymbolsPage--;        
    }

    if (InputManager::Instance()->IsButtonDown(KEY_DDOWN) && m_KeyboardTab == KeyboardTab::ACNLSymbols) {

        if (m_NinSymbolsPage == 2)
            m_NinSymbolsPage = 0;

        else m_NinSymbolsPage++;
    }

    if (InputManager::Instance()->IsButtonDown(KEY_Y)) { //Toggle Shift
        m_ShiftOn = !m_ShiftOn; //Toggle m_ShiftOn
        SetupLetters(); //UPPER <-> lower
    }

    if (InputManager::Instance()->IsButtonDown(KEY_DLEFT)) { //Remove Character
        std::string str = m_Text.GetText();
        if (!str.empty())
        {
            UTF8_String_PopBack(str);
            m_StringSize = UTF8_StringSize(str);
            m_Text = str;
        }
    }

    if (InputManager::Instance()->IsButtonDown(KEY_DRIGHT)) { //Add Space
        std::string str = m_Text.GetText();
            str.push_back(' '); //Add space
            m_StringSize = UTF8_StringSize(str);
            m_Text = str;
    }

    if (InputManager::Instance()->IsButtonDown(KEY_L)) {
        if (m_KeyboardTab == static_cast<KeyboardTab>(0))
            m_KeyboardTab = static_cast<KeyboardTab>(2);

        else --m_KeyboardTab;
        if (m_KeyboardTab == KeyboardTab::ACNLSymbols && !(m_InputType & KeyboardInType::ACNLSymbols)) //If ACNL Symbols not allowed, skip tab
            --m_KeyboardTab;

        if (m_KeyboardTab == KeyboardTab::Symbols && !(m_InputType & KeyboardInType::Symbols)) //If Symbols not allowed, skip tab
            --m_KeyboardTab;
    }

    if (InputManager::Instance()->IsButtonDown(KEY_R)) {
        if (m_KeyboardTab == static_cast<KeyboardTab>(2))
            m_KeyboardTab = static_cast<KeyboardTab>(0);

        else ++m_KeyboardTab;
        if (m_KeyboardTab == KeyboardTab::Symbols && !(m_InputType & KeyboardInType::Symbols)) //If Symbols not allowed, skip tab
            ++m_KeyboardTab;

        if (m_KeyboardTab == KeyboardTab::ACNLSymbols && !(m_InputType & KeyboardInType::ACNLSymbols)) //If ACNL Symbols not allowed, skip tab
            m_KeyboardTab = static_cast<KeyboardTab>(0);
    }

    if (InputManager::Instance()->IsButtonDown(KEY_B) && m_CanAbort) { //Abort
        m_KeyboardStatus = KeyboardStatus::Abort;
    }

    else if (InputManager::Instance()->IsButtonDown(KEY_A) || InputManager::Instance()->IsButtonDown(KEY_TOUCH)) {
        std::string str = m_Text.GetText();
        static constexpr Rect_t keyboardactivator = {{99, 161}, {225, 183}};
        if (InputManager::Instance()->IsActive(keyboardactivator)) {
            str.push_back(' ');
        }

        u8 CurIndex = 0;
        for (u32 i = 0; i < KEYBOARD_ROWS; i++)
        {
            u32 indent = 0;
            if (kbd_lyt[i] == 1) indent = 10; //'asdfghjkl' indent
            else if (kbd_lyt[i] == 3) indent = 35; //'zxcvbnm' indent
            u8 KeysPerRow = KEYS_PER_ROW - kbd_lyt[i];

            for (u32 j = 0; j < KeysPerRow; j++)
            {
                Rect_t key = {{37+(25*j)+indent, 36+(30*i)}, {37+24+(25*j)+indent, 36+29+(30*i)}};
                if (InputManager::Instance()->IsActive(key)) {
                    switch (m_KeyboardTab)
                    {
                        case KeyboardTab::Letters :
                        {
                            char letter = (m_ShiftOn && i > 0) ? (Letters[j+CurIndex]-0x20) : (Letters[j+CurIndex]);
                            str.push_back(letter);
                            break;
                        }

                        case KeyboardTab::Symbols :
                        {
                            std::u16string symbl(reinterpret_cast<const char16_t *>(Symbols + (j + CurIndex)), 1);
                            str.append(u16tou8(symbl));
                            break;
                        }

                        case KeyboardTab::ACNLSymbols :
                        {
                            std::u16string symbl(reinterpret_cast<const char16_t *>(NinSymbols + (j + CurIndex) + (KEYS_AMOUNT*m_NinSymbolsPage)), 1);
                            str.append(u16tou8(symbl));
                            break;
                        }
                    
                        default:
                            break;
                    }
                }
            }
            CurIndex += KeysPerRow;
        }

        m_StringSize = UTF8_StringSize(str);
        m_Text = str;
    }

}

void Keyboard::Update() {
    UpdateHID();
    std::string str = m_Text.GetText();
    m_StringSize = UTF8_StringSize(str);

    if (m_StringSize > m_MaxSize) { //Make sure max string length
        while (m_StringSize > m_MaxSize) {
            UTF8_String_PopBack(str);
            m_StringSize = UTF8_StringSize(str);
        }
        m_Text = str;
    }
}

KeyboardRetCode Keyboard::_Open(std::string &output)
{
    if (m_InputType == KeyboardInType::None) {
        MsgDisp(top, std::string("Keyboard Error (-2):\n\nNo Input Type has been specified."));
        return KeyboardRetCode::NoInputType;
    }

    while (aptMainLoop() && m_KeyboardStatus == KeyboardStatus::Loop) {
        Update();
        Draw();
    }

    if (m_KeyboardStatus == KeyboardStatus::Close) {    
        output = m_Text.GetText();
        return KeyboardRetCode::Success;
    }
 
    else return KeyboardRetCode::Abort;
}

KeyboardRetCode Keyboard::Open(std::string &output, u8 InType, u32 MaxSize, const std::string &DefaultText, const std::string &HintText, bool CanAbort)
{
    SetupDefaults();
    m_InputType = InType;
    m_MaxSize = MaxSize;
    m_CanAbort = CanAbort;
    m_StringSize = UTF8_StringSize(DefaultText);
    m_HintText = Text(COLOR_DARK_GREY, HintText, 1.f, 1.f);
    m_Text = Text(COLOR_WHITE, DefaultText, 1.f, 1.f);
    return _Open(output);
}

KeyboardRetCode Keyboard::Open(u32 &output, u32 MaxVal, const u32 &DefaultValue, const std::string &HintText, bool CanAbort)
{
    SetupDefaults();
    m_InputType = 0|KeyboardInType::Numbers;
    m_MaxSize = MaxVal;
    m_CanAbort = CanAbort;
    m_HintText = Text(COLOR_DARK_GREY, HintText, 1.f, 1.f);
    m_Text = Text(COLOR_WHITE, std::to_string(DefaultValue), 1.f, 1.f);
    std::string str;
    KeyboardRetCode ret = _Open(str);
    output = std::stoul(str);
    return ret;
}

KeyboardRetCode Keyboard::Open(u16 &output, u16 MaxVal, const u16 &DefaultValue, const std::string &HintText, bool CanAbort)
{
    u32 out;
    KeyboardRetCode ret = Open(out, MaxVal, DefaultValue, HintText, CanAbort);
    output = (out&0xFFFF);
    return ret;
}

KeyboardRetCode Keyboard::Open(u8 &output, u8 MaxVal, const u8 &DefaultValue, const std::string &HintText, bool CanAbort)
{
    u32 out;
    KeyboardRetCode ret = Open(out, MaxVal, DefaultValue, HintText, CanAbort);
    output = (out&0xFF);
    return ret;
}

KeyboardTab& operator++(KeyboardTab& tab)
{
    switch(tab)
    {
        case KeyboardTab::Letters:     return tab = KeyboardTab::Symbols;
        case KeyboardTab::Symbols:     return tab = KeyboardTab::ACNLSymbols;
        case KeyboardTab::ACNLSymbols: return tab = KeyboardTab::Letters;
    }
    return tab = KeyboardTab::Letters;
}

KeyboardTab& operator--(KeyboardTab& tab)
{
    switch(tab)
    {
        case KeyboardTab::Letters:     return tab = KeyboardTab::ACNLSymbols;
        case KeyboardTab::Symbols:     return tab = KeyboardTab::Letters;
        case KeyboardTab::ACNLSymbols: return tab = KeyboardTab::Symbols;
    }
    return tab = KeyboardTab::Letters;
}