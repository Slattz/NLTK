#include <3ds.h>
#include <string>
#include <citro2d.h>
#include <algorithm>
#include "CTRFont.hpp"
#include "unicode.h"
#include "gfx.h"
#include "save.h"
#include "e_utils.h"
#include "utils.h"
#include "InputManager.h"
#include "menus.h"
#include "gui/PlayerMenu.hpp"

extern InputManager *input;

PlayerSettings EditorConfig;

void Editor::Player::Draw_PlayerMenuTop(Save *saveFile, int selectedplayer) {
    static bool Init = false;
    static Text LButton;
    static Text RButton;
    static std::vector<Text> PlayerNameText;
    static std::vector<Text> PlayerNumText;
    u32 Ptext_colour;

    if (!Init)
    {
        LButton = Text(COLOR_GREY, FONT_NDS_L, 1.3f, 1.3f, 5.f, 210.f);
        RButton = Text(COLOR_GREY, FONT_NDS_R, 1.3f, 1.3f, 375.f, 210.f);
        for (int i = 0; i < 4; i++) {
            if (saveFile->players[i].Exists(saveFile)) {
                PlayerNameText.push_back(Text(COLOR_GREY, u16tou8(saveFile->players[i].Name).c_str(), 0.5f, 0.5f));
                PlayerNameText[i].CenterInBounds(18.f + (i * 100.f), 150.f, 64.f, 15.f);

                PlayerNumText.push_back(Text(COLOR_GREY, Format("Player %d", i + 1).c_str(), 0.5f, 0.5f));
                PlayerNumText[i].CenterInBounds(18.f + (i * 100.f), 25.f, 64.f, 20.f);
            }
        }
        Init = true;
    }

    draw_base_interface();

    for (int i = 0; i < 4; i++) {
        if (saveFile->players[i].Exists(saveFile)) {
            C2D_DrawImageAt(saveFile->players[i].m_TPCPic, (float)(100 * i) + 18.f, 45.f, 0.5f, nullptr, 1.f, 1.f);  //WxH: 64x104

            if (i == selectedplayer)
                Ptext_colour = COLOR_WHITE;
            else
                Ptext_colour = COLOR_GREY;

            PlayerNameText[i].SetColor(Ptext_colour).Draw();
            PlayerNumText[i].SetColor(Ptext_colour).Draw();

        }
    }
    LButton.SetColor(EditorConfig.LColor).Draw();
    RButton.SetColor(EditorConfig.RColor).Draw();
}

void Editor::Draw_PlayerMenu(Save *saveFile, int selectedplayer, int selectedmode)
{
    u32 ButtonIcon[] = {PLYR_ABOUT, PLYR_INVENTORY, PLYR_APPEARANCE, PLYR_HOUSE,
                        PLYR_PATTERN, PLYR_MAILBOX, PLYR_MAILBOX_UNREAD};
    const char* ButtonText[] = {"Info", "Inventory", "Appearance",
                            "House", "Patterns", "Mail"};

    Editor::Player::Draw_PlayerMenuTop(saveFile, selectedplayer);
    C2D_SceneBegin(bottom);

    /* Sidebar Stuff */
    C2D_DrawRectSolid(0, 0, 0, 58, 240, C2D_Color32(39,163,49,255)); //Sidebar Background
    if (selectedmode != -1 && selectedmode < 6)
        C2D_DrawRectSolid(0, 4+(40*selectedmode), 0, 55, 40, C2D_Color32(44,183,53,255)); //

    C2D_DrawRectSolid(55, 0, 0, 3, 240, COLOR_MENU_BARS); //Vertical Right Bar
    C2D_DrawRectSolid(0, 0, 0, 55, 3, COLOR_MENU_BARS); //Horizontal Top Bar

    for (int i = 0; i < 6; i++)
    {
        /* Icons */
        if (i == 5 && CheckUnreadMail(selectedplayer))
            DrawSprite(Players_ss, ButtonIcon[i+1], 15, 4+(40*i)); //If Unread Mail

        else
            DrawSprite(Players_ss, ButtonIcon[i], 15, 4+(40*i));

        /* Text */
        if (i == 1)
            draw_centered_text(1, 53, 35+(39*i), 8, 0.38, 0.38, COLOR_GREY, ButtonText[i]); //Appearance Text

        else if (i == 2)
            draw_centered_text(1, 53, 35+(39*i), 8, 0.35, 0.38, COLOR_GREY, ButtonText[i]); //Inventory Text

        else
            draw_centered_text(1, 53, 35+(39*i), 8, 0.4, 0.4, COLOR_GREY, ButtonText[i]); //Text
    }

    input->DrawCursor();
    C3D_FrameEnd(0);
}

void Editor::Spawn_PlayerMenu(Save *saveFile)
{
    if (EditorConfig.DrawingMenu)
        return;

    EditorConfig.DrawingMenu = true;

    static int selectedmode = 0;

    while (aptMainLoop())
    {
        checkIfCardInserted();

        Editor::Draw_PlayerMenu(saveFile, EditorConfig.SelectedPlayer, selectedmode);
        input->RefreshInput();

        EditorConfig.LColor = EditorConfig.RColor = COLOR_GREY;

        if (hidKeysDown() & KEY_B)
            break;

        if (hidKeysHeld() & KEY_R)
            EditorConfig.RColor = COLOR_WHITE;

        if (hidKeysHeld() & KEY_L)
            EditorConfig.LColor = COLOR_WHITE;

        if (hidKeysDown() & KEY_R)
        {
            while (true) {
                EditorConfig.SelectedPlayer++;
                if (EditorConfig.SelectedPlayer > 3)
                    EditorConfig.SelectedPlayer = 0;

                if (saveFile->players[EditorConfig.SelectedPlayer].Exists(saveFile))
                {
                    break;
                }
            }
        }

        if (hidKeysDown() & KEY_L)
        {
            while (true) {
                EditorConfig.SelectedPlayer--;
                if (EditorConfig.SelectedPlayer < 0)
                    EditorConfig.SelectedPlayer = 3;

                if (saveFile->players[EditorConfig.SelectedPlayer].Exists(saveFile))
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
        if (input->IsActive(plyr_info)) //Info
        {
            selectedmode = 0;
            Editor::Player::Spawn_PlayerMenu_Info(saveFile);
        }
        else if (input->IsActive(plyr_inv)) //Inv
        {
            selectedmode = 1;
            Editor::Player::Spawn_PlayerMenu_Inventory(saveFile);
        }
        else if (input->IsActive(plyr_looks)) //Appearance
        {
            selectedmode = 2;
        }
        else if (input->IsActive(plyr_house)) //House
        {
            selectedmode = 3;
        }
        else if (input->IsActive(plyr_pat)) //Patterns
        {
            selectedmode = 4;
            Editor::Player::Spawn_PlayerMenu_Patterns(saveFile);
        }
        else if (input->IsActive(plyr_mail)) //Mail
        {
            selectedmode = 5;
        }
    }

    EditorConfig.DrawingMenu = false;
}