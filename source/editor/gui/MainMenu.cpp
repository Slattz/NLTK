#include <3ds.h>
#include <string>
#include <citro2d.h>
#include <algorithm>
#include "CTRFont.hpp"
#include "gfx.h"
#include "save.h"
#include "e_utils.h"
#include "utils.h"
#include "cursor.h"
#include "menus.h"
#include "e_menus.h"
#include "gui/MainMenu.hpp"
#include "gui/PlayerMenu.hpp"

extern Cursor g_cursor;
extern s16  g_CheckX[2];
extern s16  g_CheckY[2];
extern bool g_disabled[2];
extern u32  g_key[2];

void Editor::Draw_MainMenu(void)
{
    static bool TextInit = false;
    static const float TextSize = 0.6f;
    static std::vector<Text> ButtonText;
    static std::vector<Text> ColumnText;
    static int ButtonIcon[] = {TOWN_ICON, ACRES_ICON, PROFILE_ICON, VILLAGERS_ICON,
                               PWP_ICON, ISLAND_ICON, MAIN_STREET_ICON, ABOUT_ICON};

    if (!TextInit) {
        ButtonText.push_back(Text(COLOR_GREY, "Town", TextSize, TextSize, 37.f, 147.f));
        ButtonText.push_back(Text(COLOR_GREY, "Acres", TextSize, TextSize, 110.f, 147.f));
        ButtonText.push_back(Text(COLOR_GREY, "Players", TextSize, TextSize, 178.f, 147.f));
        ButtonText.push_back(Text(COLOR_GREY, "Villagers", TextSize, TextSize, 248.f, 147.f));
        ButtonText.push_back(Text(COLOR_GREY, "PWPs", TextSize, TextSize, 37.f, 217.f));
        ButtonText.push_back(Text(COLOR_GREY, "Island", TextSize, TextSize, 110.f, 217.f));
        ButtonText.push_back(Text(COLOR_GREY, "Main Street", 0.55f, 0.55f, 168.f, 217.f));
        ButtonText.push_back(Text(COLOR_GREY, "About", TextSize, TextSize, 256.f, 217.f));

        ColumnText.push_back(Text(COLOR_GREY, "Game\nSelect", TextSize, TextSize, 44.f, 12.f));
        ColumnText.push_back(Text(COLOR_GREY, "  Town\nManager", TextSize, TextSize, 40.f, 60.f));
        ColumnText.push_back(Text(COLOR_GREY, "Save", TextSize, TextSize, 248.f, 18.f));
        ColumnText.push_back(Text(COLOR_GREY, "Options", TextSize, TextSize, 240.f, 67.f));
        TextInit = true;
    }

    draw_base_interface();
    C2D_SceneBegin(bottom);
    DrawSprite(Common_ss, NLTK_ICON, 126, 10); //NLTK's Icon

    for (int i = 0; i < 2; i++)
    {
        DrawSprite(Editor_ss, BUTTON_MAIN, 20,  10+(50*i), nullptr, 1.15, 0.6);  //w = 80, h = 33
        DrawSprite(Editor_ss, BUTTON_MAIN, 220, 10+(50*i), nullptr, 1.15, 0.6);  //w = 80, h = 33
        ColumnText[i].Draw(); //Column 1 Text
        ColumnText[2+i].Draw(); //Column 2 Text
    }

    for (int i = 0; i < 4; i++)
    {
        DrawSprite(Editor_ss, BUTTON_MAIN, 15+(74*i), 110); //Row 1 Buttons
        DrawSprite(Editor_ss, BUTTON_MAIN, 15+(74*i), 180); //Row 2 Buttons
        DrawSprite(Editor_ss, ButtonIcon[i], 39+(74*i), 114, GreenFilter); //Row 1 Icons
        DrawSprite(Editor_ss, ButtonIcon[4+i], 39+(74*i), 184, GreenFilter); //Row 2 Icons
        ButtonText[i].Draw(); //Row 1 Text
        ButtonText[i+4].Draw(); //Row 2 Text
    }

    C2D_SceneBegin(top);
    draw_centered_text(0, 400, 80, 0, 1.1, 1.1, COLOR_GREY, "Editor Main Menu!");
    g_cursor.Draw();
    C3D_FrameEnd(0);
}

int Editor::Spawn_MainMenu(Save *saveFile)
{
    while (aptMainLoop())
    {
        checkIfCardInserted();

        hidScanInput();
        Editor::Draw_MainMenu();
        updateCursorInfo();

        if (hidKeysDown() & KEY_START)
        {
            return MODE_EXIT;
        }

        else if (hidKeysDown() & KEY_B)
        {
            return MODE_MAINMENU;
        }

        for (int i = 0; i < 2; i++)
        {
            if (hidKeysDown() & g_key[i] && g_disabled[i])
            {
                /* Menu Buttons */
                if (g_CheckX[i] >= 15 && g_CheckX[i] <= 85 && g_CheckY[i] >= 110 && g_CheckY[i] <= 165) //Town Menu
                    MsgDisp(top, "Town Menu Coming Soon!");

                else if (g_CheckX[i] >= 89 && g_CheckX[i] <= 159 && g_CheckY[i] >= 110 && g_CheckY[i] <= 165) //Acres Menu
                    spawn_acres_menu(saveFile);

                else if (g_CheckX[i] >= 163 && g_CheckX[i] <= 233 && g_CheckY[i] >= 110 && g_CheckY[i] <= 165) //Player Menu
                    Editor::Spawn_PlayerMenu(saveFile);

                else if (g_CheckX[i] >= 237 && g_CheckX[i] <= 307 && g_CheckY[i] >= 110 && g_CheckY[i] <= 165) //Villagers Menu
                    MsgDisp(top, "Villagers Menu Coming Soon!");

                else if (g_CheckX[i] >= 15 && g_CheckX[i] <= 85 && g_CheckY[i] >= 180 && g_CheckY[i] <= 235) //Pwps Menu
                    MsgDisp(top, "PWP Menu Coming Soon!");

                else if (g_CheckX[i] >= 89 && g_CheckX[i] <= 159 && g_CheckY[i] >= 180 && g_CheckY[i] <= 235) //Island Menu
                    MsgDisp(top, "Island Menu Coming Soon!");

                else if (g_CheckX[i] >= 163 && g_CheckX[i] <= 233 && g_CheckY[i] >= 180 && g_CheckY[i] <= 235) //Main Street Menu
                    MsgDisp(top, "Main Street Menu Coming Soon!");

                else if (g_CheckX[i] >= 237 && g_CheckX[i] <= 307 && g_CheckY[i] >= 180 && g_CheckY[i] <= 235) //About Menu
                    spawn_about_menu();

                /* Menu Columns */
                else if (g_CheckX[i] >= 220 && g_CheckX[i] <= 300 && g_CheckY[i] >= 10 && g_CheckY[i] <= 44)
                { //Save Column
                    if (check_if_saving())
                    {
                        if (saveFile->Commit(false))
                        {
                            MsgDisp(top, "Save Succeeded!");
                        }
                        else
                        {
                            MsgDisp(top, "Save Failed!");
                        }
                    }
                }
                else if (g_CheckX[i] >= 220 && g_CheckX[i] <= 300 && g_CheckY[i] >= 60 && g_CheckY[i] <= 99) //Options Column
                    spawn_config_menu();

                else if (g_CheckX[i] >= 20 && g_CheckX[i] <= 100 && g_CheckY[i] >= 10 && g_CheckY[i] <= 44)
                { //Game Select Column
                    return MODE_GAMESELECT;
                }

                else if (g_CheckX[i] >= 20 && g_CheckX[i] <= 100 && g_CheckY[i] >= 60 && g_CheckY[i] <= 99) //Town Manager Column
                    return MODE_MANAGER;
            }
        }
    }
    return 0;
}
