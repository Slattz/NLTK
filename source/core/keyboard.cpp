#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "gfx.h"
#include "utils.h"
#include "keyboard.h"
#include "cursor.h"

#define KEYS_PER_ROW 10
#define KEYBOARD_ROWS 4

extern Cursor g_cursor;

static const u32 SWKBD_BG = C2D_Color32(0x21, 0x8B, 0x2B, 0xFF);
static const u32 SWKBD_BAR = C2D_Color32(0x14, 0x56, 0x1A, 0xFF);
static const u32 SWKBD_TAB_CLR = C2D_Color32(0x35, 0xCC, 0x1A, 0xFF);
static const u32 SWKBD_CHAR_BG = C2D_Color32(0x19, 0x68, 0x1F, 0xFF);

const char Letters[] = "1234567890qwertyuiopasdfghjklzxcvbnmOOBTEXT";
const wchar_t Symbols[] = L"@#$_&-+=!?*\"\':;()<>%&[]{}▲▼◀▶£€|\\/♂♀OOBSYML"; //Needs to be wchar_t as £ and € are utf-16 :/
const char16_t NinSymbols[] = { //Laid out in same layout as characters per keyboard row: 10,10,9,7
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
    0xE064, 0xE065, 0xE067, 0xE073, 0xE074, 0xE076, 0xE075, //End of page 3         //7
    0x004F, 0x004F, 0x0042, 0x004E, 0x0049, 0x004E}; //OOBNIN

Keyboard::Keyboard(void)
{
    m_InputType = KType_None;
    m_MaxSize = 100;
    m_CanAbort = true;
    m_ShiftOn = false;
    m_NinSymbolsPage = 0;
    m_KeyboardStatus = KeyboardStatus::Loop;
    m_KeyboardTab = KTab_Letters;
    SetupLetters();
    SetupSymbols();
    SetupACNLSymbols();
}

Keyboard::~Keyboard(void) {
    m_Characters.clear();
    m_Symbols.clear();
    m_ACNLSymbols.clear();
    m_Characters.shrink_to_fit();
    m_Symbols.shrink_to_fit();
    m_ACNLSymbols.shrink_to_fit();
}

Keyboard::Keyboard(u8 InType, u32 MaxSize, bool CanAbort) {
    m_InputType = InType;
    m_MaxSize = MaxSize;
    m_CanAbort = CanAbort;
    m_ShiftOn = false;
    m_NinSymbolsPage = 0;
    m_KeyboardStatus = KeyboardStatus::Loop;
    m_KeyboardTab = KTab_Letters;
    SetupCommonText();
    SetupLetters();
    SetupSymbols();
    SetupACNLSymbols();
}
Keyboard::Keyboard(u8 InType, u32 MaxSize, bool CanAbort, const std::string &HintText, const std::string &DefaultText)
    : m_HintText(COLOR_DARK_GREY, HintText, 1.f, 1.f), m_Text(COLOR_WHITE, DefaultText, 1.f, 1.f)
{
    m_InputType = InType;
    m_MaxSize = MaxSize;
    m_CanAbort = CanAbort;
    m_ShiftOn = false;
    m_NinSymbolsPage = 0;
    m_KeyboardStatus = KeyboardStatus::Loop;
    m_KeyboardTab = KTab_Letters;
    SetupCommonText();
    SetupLetters();
    SetupSymbols();
    SetupACNLSymbols();
}
Keyboard::Keyboard(u32 MaxSize, bool CanAbort, const std::string &HintText, const u32 &DefaultValue)
    : m_HintText(COLOR_DARK_GREY, HintText, 1.f, 1.f), m_Text(COLOR_WHITE, std::to_string(DefaultValue), 1.f, 1.f)
{
    m_InputType = KType_Numbers;
    m_MaxSize = MaxSize;
    m_CanAbort = CanAbort;
    m_ShiftOn = false;
    m_NinSymbolsPage = 0;
    m_KeyboardStatus = KeyboardStatus::Loop;
    m_KeyboardTab = KTab_Letters;
    SetupCommonText();
    SetupLetters();
    SetupSymbols();
    SetupACNLSymbols();
}

void Keyboard::SetupCommonText() {
    Text Comma(COLOR_WHITE, ",", 1.0f, 1.0f, 85.f, 157.f);
    Text FStop(COLOR_WHITE, ".", 1.0f, 1.0f, 237.f, 157.f);
    Text LetterTab(COLOR_WHITE, "ABC\n123", 0.8f, 0.8f);
    Text SymTab(COLOR_WHITE, "Symbols", 0.8f, 0.8f);
    Text NinSymTab(COLOR_WHITE, "Nintendo\nSymbols", 0.8f, 0.8f);
    Text Up(COLOR_GREY, "▲", 0.9f, 0.9f, 295.f, 45.f);
    Text Down(COLOR_GREY, "▼", 0.9f, 0.9f, 295.f, 95.f);
    Text Page(COLOR_GREY, "Page", 0.6f, 0.6f, 290.f, 65.f);
    Text PageNum(COLOR_GREY, "1", 0.8f, 0.8f, 297.f, 80.f);
    LetterTab.CenterInBounds(2.f, 202.f, 98.f, 38.f);
    SymTab.CenterInBounds(100.f, 202.f, 124.f, 38.f);
    NinSymTab.CenterInBounds(224.f, 202.f, 96.f, 38.f);
    m_CommonText.push_back(Comma);
    m_CommonText.push_back(FStop);
    m_CommonText.push_back(LetterTab);
    m_CommonText.push_back(SymTab);
    m_CommonText.push_back(NinSymTab);
    m_CommonText.push_back(Up);
    m_CommonText.push_back(Down);
    m_CommonText.push_back(Page);
    m_CommonText.push_back(PageNum);
}

void Keyboard::SetupLetters() {
    u8 CurIndex = 0;
    const u8 kbd_lyt[] = {0,0,1,3};
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
    const u8 kbd_lyt[] = {0,0,1,3};
    m_Symbols.clear();

    for (u32 i = 0; i < KEYBOARD_ROWS; i++) //Take 1 as only 3 rows of symbols
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
    const u8 kbd_lyt[] = {0,0,1,3};
    m_ACNLSymbols.clear();

    for(u8 page = 0; page < 3; page++)
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


void Keyboard::SetHint(const std::string &HintText)
{
    m_HintText = HintText;
}

void Keyboard::SetText(const std::string &DefaultText) {
    m_Text = DefaultText;
}

void Keyboard::SetValue(const u32 &DefaultValue) {
    m_Text = std::to_string(DefaultValue);
}

void Keyboard::CanAbort(bool canAbort) {
    m_CanAbort = canAbort;
}

void Keyboard::SetMaxLength(u32 maxSize) {
    m_MaxSize = maxSize;
}

void Keyboard::SetInputType(u8 InType) {
    m_InputType = InType;
}

void Keyboard::Draw() {
    draw_base_interface();
    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, SWKBD_BG);
    C2D_DrawRectSolid(0.f, 0.f, 0.f, 320.f, 25.f, SWKBD_BAR); //Bar for (Hint) Text Background (Top)
    C2D_DrawRectSolid(0.f, 200.f, 0.f, 320.f, 40.f, SWKBD_BAR); //Bar for Tabs and Exit (Bottom)
    C2D_DrawRectSolid(283.f, 122.f, 0.f, 33.f, 68.f, SWKBD_CHAR_BG);  //Bar for Confirm/Cancel (Right)
    C2D_DrawRectSolid(38.f, 37.f, 0.5f, 245.f, 153.f, SWKBD_CHAR_BG); //Characters BG
    C2D_DrawRectSolid(99.f, 161.f, 0.5f, 126.f, 22.f, COLOR_DARK_GREY); //Spacebar Outline
    C2D_DrawRectSolid(100.f, 162.f, 0.5f, 124.f, 20.f, COLOR_GREY);     //Spacebar
    DrawSprite(Swkbd_ss, SWKBD_BACK, 255.f, 127.f, nullptr, 0.8f, 0.8f, 0.5f); //Keyboard Back Icon
    DrawSprite(Swkbd_ss, SWKBD_ENTER, 256.f, 160.f, nullptr, 0.8f, 0.8f, 0.5f);  //Keyboard Enter Icon
    DrawSprite(Swkbd_ss, SWKBD_CONFIRM, 287.f, 127.f, nullptr, 0.8f, 0.8f); //Keyboard Confirm Icon
    DrawSprite(Swkbd_ss, SWKBD_CANCEL, 287.f, 160.f, nullptr, 0.8f, 0.8f); //Keyboard Cancel Icon
    m_CommonText[0].Draw(); //Comma
    m_CommonText[1].Draw(); //F Stop

    m_HintText.CenterInBounds(2.5f, 2.5f, 317.5f, 22.5f); //Update Hint text position + width
    m_Text.CenterInBounds(2.5f, 2.5f, 317.5f, 22.5f); //Update Text position + width
    if (m_Text.GetText().empty()) //Check if main text contains no characters
        m_HintText.Draw();
    else
        m_Text.Draw();

    switch (m_KeyboardTab) //Draw correct keys per tab
    {
        case KTab_Letters :
            for (u32 i = 0; i < m_Characters.size(); i++)
                m_Characters[i].Draw();

            if (m_ShiftOn) DrawSprite(Swkbd_ss, SWKBD_SHIFT_ON, 45.f, 128.f, nullptr, 0.8f, 0.8f, 0.5f); //Keyboard Shift Icon
            else  DrawSprite(Swkbd_ss, SWKBD_SHIFT_OFF, 45.f, 128.f, nullptr, 0.8f, 0.8f, 0.5f); //Keyboard Shift Icon
            C2D_DrawRectSolid(0.f, 200.f, 0.f, 100.f, 40.f, SWKBD_TAB_CLR);    //Highlight selected tab
            break;

        case KTab_Symbols :
            for (u32 i = 0; i < m_Symbols.size(); i++)
                m_Symbols[i].Draw();

            C2D_DrawRectSolid(100.f, 200.f, 0.f, 124.f, 40.f, SWKBD_TAB_CLR);    //Highlight selected tab
            break;

        case KTab_ACNLSymbols :
            for (u32 i = 0; i < m_ACNLSymbols.size()/3; i++) {
                m_ACNLSymbols[i + (m_ACNLSymbols.size()/3 * m_NinSymbolsPage)].Draw(); //Should be 36 is amount of characters in one page
            }

            C2D_DrawRectSolid(224.f, 200.f, 0.f, 98.f, 40.f, SWKBD_TAB_CLR);   //Highlight selected tab
            m_CommonText[8] = std::to_string(m_NinSymbolsPage+1);              //Update page number text

            for (int i = 0; i < 4; i++)
                m_CommonText[i+5].Draw(); //Draw text for pages (arrow up, arrow down, "Page", page num)
            break;

        default:
            break;
    }

    for (int i = 0; i < 3; i++)
        m_CommonText[i+2].Draw(); //Draw text for the tabs (+2 as comma and fstop before)

    g_cursor.Draw();

    C3D_FrameEnd(0);
}
void Keyboard::UpdateHID() {
    u32 kDown = hidKeysDown();
    updateCursorInfo(); //Update cursor info

    if (kDown & KEY_DUP && m_KeyboardTab == KTab_ACNLSymbols) {

        if (m_NinSymbolsPage == 0)
            m_NinSymbolsPage = 2;

        else m_NinSymbolsPage--;        
    }

    if (kDown & KEY_DDOWN && m_KeyboardTab == KTab_ACNLSymbols) {

        if (m_NinSymbolsPage == 2)
            m_NinSymbolsPage = 0;

        else m_NinSymbolsPage++;
    }

    if (kDown & KEY_Y) { //Toggle Shift
        m_ShiftOn = !m_ShiftOn; //Toggle m_ShiftOn
        SetupLetters(); //UPPER <-> lower
    }

    if (kDown & KEY_DLEFT) { //Remove Character
        std::string str = m_Text.GetText();
        if (!str.empty())
        {
            str.pop_back();
            m_Text = str;
        }
    }

    if (kDown & KEY_DRIGHT) { //Add Space
        std::string str = m_Text.GetText();
            str.push_back(' '); //Add space
            m_Text = str;
    }

    if (kDown & KEY_A || kDown & KEY_TOUCH) {

    }

    if (kDown & KEY_L) {
        if (m_KeyboardTab == 0)
            m_KeyboardTab = 2;
       
        else m_KeyboardTab--;
    }

    if (kDown & KEY_R) {
        if (m_KeyboardTab == 2)
            m_KeyboardTab = 0;
       
        else m_KeyboardTab++;
    }

    if (kDown & KEY_B && m_CanAbort) //Abort
    {
        m_KeyboardStatus = KeyboardStatus::Abort;
    }
}

void Keyboard::Update() {
    UpdateHID();
    std::string str = m_Text.GetText();

    if (str.length() > m_MaxSize) { //Make sure max string length
        while (str.length() > m_MaxSize) {
            str.pop_back();
        }
        m_Text = str;
    }
}

int Keyboard::_Open(std::string &output)
{
    while (aptMainLoop() && m_KeyboardStatus == KeyboardStatus::Loop) {
        Update();
        Draw();
    }

    return -1;
}

int Keyboard::Open(std::string &output) {
    int res = _Open(output);
    return res;
}

int Keyboard::Open(u32& output) {
    std::string str = std::to_string(output);
    int res = _Open(str);
    output = stoul(str);
    return res;
}