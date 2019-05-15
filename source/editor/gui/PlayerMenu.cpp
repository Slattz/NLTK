#include <3ds.h>
#include <string>
#include <citro2d.h>
#include <algorithm>
#include "CTRFont.hpp"
#include "InputManager.h"
#include "unicode.h"
#include "label.h"
#include "imagebutton.h"
#include "gfx.h"
#include "save.h"
#include "e_utils.h"
#include "utils.h"
#include "menus.h"
#include "gui/PlayerMenu.hpp"

PlayerSettings PlayerConfig;

//Top
static Label* LButton = nullptr;
static Label* RButton = nullptr;
static std::vector<Label *> PlayerNameText;
static std::vector<Label *> PlayerNumText;
//static std::vector<ImageButton *> MenuButtons;

//Player Menu
static Label* PInfo = nullptr;
static Label* PInv = nullptr;
static Label* PAppearance = nullptr;
static Label* PHouse = nullptr;
static Label* PPatterns = nullptr;
static Label* PMail = nullptr;

void Editor::Player::InitializeTop(void) {
    LButton = new Label({5, 210}, {0, 0}, COLOR_TRANSPARENT, COLOR_GREY, FONT_NDS_L);
    LButton->SetTextSize(1.1f, 1.1f);

    RButton = new Label({370, 210}, {0, 0}, COLOR_TRANSPARENT, COLOR_GREY, FONT_NDS_R);
    RButton->SetTextSize(1.1f, 1.1f);

    for (int i = 0; i < 4; i++) {
        if (Save::Instance()->players[i]->Exists()) {
            Label *NameText = new Label({0, 0}, {0, 0}, COLOR_TRANSPARENT, COLOR_GREY, u16tou8(Save::Instance()->players[i]->Name).c_str());
            NameText->SetTextSize(0.45f, 0.45f);
            NameText->CenterInBounds(18.f + (i * 100.f), 150.f, 64.f, 15.f);
            PlayerNameText.push_back(NameText);

            Label *NumText = new Label({0, 0}, {0, 0}, COLOR_TRANSPARENT, COLOR_GREY, Format("Player %d", i + 1).c_str());
            NumText->SetTextSize(0.5f, 0.5f);
            NumText->CenterInBounds(18.f + (i * 100.f), 25.f, 64.f, 20.f);
            PlayerNumText.push_back(NumText);
        }
    }
    PlayerConfig.TopMenuInit = true;
}

void Editor::Player::UpdateTopInfo(void) {
    PlayerNameText[PlayerConfig.SelectedPlayer]->myText = u16tou8(Save::Instance()->players[PlayerConfig.SelectedPlayer]->Name);
    PlayerNameText[PlayerConfig.SelectedPlayer]->CenterInBounds(18.f + (PlayerConfig.SelectedPlayer * 100.f), 150.f, 64.f, 15.f);
}

void Editor::Player::Initialize(void) {
    if (!PlayerConfig.TopMenuInit) Editor::Player::InitializeTop();

    PInfo = new Label({0, 0}, {0, 0}, COLOR_TRANSPARENT, COLOR_GREY, "Info");
    PInv = new Label({0, 0}, {0, 0}, COLOR_TRANSPARENT, COLOR_GREY, "Inventory");
    PAppearance = new Label({0, 0}, {0, 0}, COLOR_TRANSPARENT, COLOR_GREY, "Appearance");
    PHouse = new Label({0, 0}, {0, 0}, COLOR_TRANSPARENT, COLOR_GREY, "House");
    PPatterns = new Label({0, 0}, {0, 0}, COLOR_TRANSPARENT, COLOR_GREY, "Patterns");
    PMail = new Label({0, 0}, {0, 0}, COLOR_TRANSPARENT, COLOR_GREY, "Mail");

    PInfo->SetTextSize(0.4f, 0.4f);
    PInv->SetTextSize(0.38f, 0.38f);
    PAppearance->SetTextSize(0.35f, 0.38f);
    PHouse->SetTextSize(0.4f, 0.4f);
    PPatterns->SetTextSize(0.4f, 0.4f);
    PMail->SetTextSize(0.4f, 0.4f);

    PInfo->CenterInBounds(1.f, 35.f, 53.f, 8.f);
    PInv->CenterInBounds(1.f, 74.f, 53.f, 8.f);
    PAppearance->CenterInBounds(1.f, 113.f, 53.f, 8.f);
    PHouse->CenterInBounds(1.f, 152.f, 53.f, 8.f);
    PPatterns->CenterInBounds(1.f, 191.f, 53.f, 8.f);
    PMail->CenterInBounds(1.f, 230.f, 53.f, 8.f);
}

void Editor::Player::CleanupTop(void) {
    delete LButton;
    delete RButton;
    LButton = nullptr;
    RButton = nullptr;

    for (auto name : PlayerNameText) {
        delete name;
        name = nullptr;
    }

    for (auto num : PlayerNumText) {
        delete num;
        num = nullptr;
    }

    PlayerNameText.clear();
    PlayerNumText.clear();

    PlayerConfig.TopMenuInit = false;
}

void Editor::Player::Cleanup(void) {
    if (PlayerConfig.TopMenuInit) Editor::Player::CleanupTop();

    delete PInfo;
    delete PInv;
    delete PAppearance;
    delete PHouse;
    delete PPatterns;
    delete PMail;

    PInfo = nullptr;
    PInv = nullptr;
    PAppearance = nullptr;
    PHouse = nullptr;
    PPatterns = nullptr;
    PMail = nullptr;
}

void Editor::Player::Draw_PlayerMenuTop(void) {
    u32 Ptext_colour = COLOR_GREY;

    draw_base_interface();

    for (int i = 0; i < 4; i++) {
        if (Save::Instance()->players[i]->Exists()) {
            C2D_DrawImageAt(Save::Instance()->players[i]->m_TPCPic, (float)(100 * i) + 18.f, 45.f, 0.5f, nullptr, 1.f, 1.f);  //WxH: 64x104

            if (i == PlayerConfig.SelectedPlayer)
                Ptext_colour = COLOR_WHITE;

            else Ptext_colour = COLOR_GREY;

            PlayerNameText[i]->SetTextColor(Ptext_colour);
            PlayerNameText[i]->Draw();
            PlayerNumText[i]->SetTextColor(Ptext_colour);
            PlayerNumText[i]->Draw();

        }
    }

    LButton->SetTextColor(PlayerConfig.LColor);
    RButton->SetTextColor(PlayerConfig.RColor);
    LButton->Draw();
    RButton->Draw();
}

void Editor::Draw_PlayerMenu(void)
{
    const u32 ButtonIcon[] = {PLYR_ABOUT, PLYR_INVENTORY, PLYR_APPEARANCE, PLYR_HOUSE,
                                PLYR_PATTERN, PLYR_MAILBOX, PLYR_MAILBOX_UNREAD};

    Editor::Player::Draw_PlayerMenuTop();
    C2D_SceneBegin(bottom);

    /* Sidebar Stuff */
    C2D_DrawRectSolid(0, 0, 0, 58, 240, C2D_Color32(39,163,49,255)); //Sidebar Background

    C2D_DrawRectSolid(55, 0, 0, 3, 240, COLOR_MENU_BARS); //Vertical Right Bar
    C2D_DrawRectSolid(0, 0, 0, 55, 3, COLOR_MENU_BARS); //Horizontal Top Bar

    PInfo->Draw();
    PInv->Draw();
    PAppearance->Draw();
    PHouse->Draw();
    PPatterns->Draw();
    PMail->Draw();

    for (int i = 0; i < 6; i++)
    {
        /* Icons */
        if (i == 5 && CheckUnreadMail(PlayerConfig.SelectedPlayer))
            DrawSprite(Players_ss, ButtonIcon[i+1], 15, 4+(40*i)); //If Unread Mail

        else
            DrawSprite(Players_ss, ButtonIcon[i], 15, 4+(40*i));
    }

    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

void Editor::Spawn_PlayerMenu()
{
    if (PlayerConfig.DrawingMenu)
        return;

    PlayerConfig.DrawingMenu = true;

    Editor::Player::Initialize();

    while (aptMainLoop())
    {
        checkIfCardInserted();

        Editor::Draw_PlayerMenu();
        InputManager::Instance()->RefreshInput();

        PlayerConfig.LColor = PlayerConfig.RColor = COLOR_GREY;

        if (InputManager::Instance()->IsButtonDown(KEY_B))
            break;

        if (InputManager::Instance()->IsButtonHeld(KEY_R))
            PlayerConfig.RColor = COLOR_WHITE;

        if (InputManager::Instance()->IsButtonHeld(KEY_L))
            PlayerConfig.LColor = COLOR_WHITE;

        if (InputManager::Instance()->IsButtonDown(KEY_R))
        {
            while (true) {
                PlayerConfig.SelectedPlayer++;
                if (PlayerConfig.SelectedPlayer > 3)
                    PlayerConfig.SelectedPlayer = 0;

                if (Save::Instance()->players[PlayerConfig.SelectedPlayer]->Exists())
                {
                    break;
                }
            }
        }

        if (InputManager::Instance()->IsButtonDown(KEY_L))
        {
            while (true) {
                PlayerConfig.SelectedPlayer--;
                if (PlayerConfig.SelectedPlayer < 0)
                    PlayerConfig.SelectedPlayer = 3;

                if (Save::Instance()->players[PlayerConfig.SelectedPlayer]->Exists())
                {
                    break;
                }
            }
        }

        static const Rect_t plyr_info = {{0, 0}, {58, 40}};
        static const Rect_t plyr_inv = {{0, 40}, {58, 80}};
        static const Rect_t plyr_looks = {{0, 80}, {58, 120}};
        static const Rect_t plyr_house = {{0, 120}, {58, 160}};
        static const Rect_t plyr_pat = {{0, 160}, {58, 200}};
        static const Rect_t plyr_mail = {{0, 200}, {58, 240}};

        /* Menu Buttons */
        if (InputManager::Instance()->IsActive(plyr_info)) //Info
        {
            Editor::Player::Spawn_PlayerMenu_Info();
        }
        else if (InputManager::Instance()->IsActive(plyr_inv)) //Inv
        {
            Editor::Player::Spawn_PlayerMenu_Inventory();
        }
        else if (InputManager::Instance()->IsActive(plyr_looks)) //Appearance
        {
        }
        else if (InputManager::Instance()->IsActive(plyr_house)) //House
        {
        }
        else if (InputManager::Instance()->IsActive(plyr_pat)) //Patterns
        {
            Editor::Player::Spawn_PlayerMenu_Patterns();
        }
        else if (InputManager::Instance()->IsActive(plyr_mail)) //Mail
        {
        }
    }
    Editor::Player::Cleanup();
    PlayerConfig.DrawingMenu = false;
}