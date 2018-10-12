#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "CTRFont.hpp"
#include "gfx.h"
#include "save.h"
#include "item.h"
#include "e_utils.h"
#include "utils.h"
#include "cursor.h"
#include "menus.h"
#include "gui/PlayerMenu.hpp"

extern Cursor g_cursor;
extern s16 g_CheckX[2];
extern s16 g_CheckY[2];
extern bool g_disabled[2];
extern u32 g_key[2];

static void Draw_PlayerMenu_Inventory(Save *saveFile, int selectedplayer)
{
    int x = 42;
    int y = 63;

    Editor::Player::Draw_PlayerMenuTop(saveFile, selectedplayer);
    C2D_SceneBegin(bottom);

    for (int i = 0; i < 16; ++i)
    {
        if (i == 2)
        {
            x += 38 * 2;
        }

        if (i > 0 && (i == 4 || i % 10 == 0))
        {
            y += 38;
            x = 42;
        }

        Item item = saveFile->players[selectedplayer].Pockets[i];
        DrawSprite(Common_ss, ITEM_HOLE, x - 16, y - 16);

        if (item.Icon > -1)
        {
            DrawSprite(Items_ss, item.Icon, x, y);
        }

        x += 38;
    }

    g_cursor.Draw();
    C3D_FrameEnd(0);
}

void Editor::Player::Spawn_PlayerMenu_Inventory(Save *saveFile) {
    if (EditorConfig.DrawingSubmenu)
        return;

    EditorConfig.DrawingSubmenu = true;

    load_player_invitems(saveFile, EditorConfig.SelectedPlayer);

    while (aptMainLoop())
    {
        checkIfCardInserted();

        hidScanInput();
        Draw_PlayerMenu_Inventory(saveFile, EditorConfig.SelectedPlayer);
        updateCursorInfo();

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

                if (saveFile->players[EditorConfig.SelectedPlayer].Exists(saveFile)) {
                    break;
                }
            }

            load_player_invitems(saveFile, EditorConfig.SelectedPlayer);
        }

        if (hidKeysDown() & KEY_L)
        {    
            while (true) {
                EditorConfig.SelectedPlayer--;
                if (EditorConfig.SelectedPlayer < 0)
                    EditorConfig.SelectedPlayer = 3;

                if (saveFile->players[EditorConfig.SelectedPlayer].Exists(saveFile)) {
                    break;
                }
            }

            load_player_invitems(saveFile, EditorConfig.SelectedPlayer);
        }
    }

    EditorConfig.DrawingSubmenu = false;
}