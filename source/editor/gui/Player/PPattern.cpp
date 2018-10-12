#include <3ds.h>
#include <string>
#include <citro2d.h>
#include <algorithm>
#include "CTRFont.hpp"
#include "gfx.h"
#include "save.h"
#include "pattern.h"
#include "e_utils.h"
#include "utils.h"
#include "cursor.h"
#include "menus.h"
#include "e_menus.h"
#include "gui/PlayerMenu.hpp"

extern Cursor g_cursor;
extern s16 g_CheckX[2];
extern s16 g_CheckY[2];
extern bool g_disabled[2];
extern u32 g_key[2];

static void Draw_PlayerMenu_Patterns(Save *saveFile, int selectedPlayer)
{
    Editor::Player::Draw_PlayerMenuTop(saveFile, selectedPlayer);
    C2D_SceneBegin(bottom);

    float x = 20;
    float y = 20;

    for (int i = 0; i < 10; i++) {

        if (i > 0 && i % 2 == 0) {
            x = 20;
            y += 40;
        }
        else if (i > 0) {
            x += 40;
        }

        C2D_DrawImageAt(saveFile->players[selectedPlayer].Patterns[i]->Images[0], x, y, 0.5f, nullptr, 1.0f, 1.0f);
    }

    g_cursor.Draw();
    C3D_FrameEnd(0);
}

void Editor::Player::Spawn_PlayerMenu_Patterns(Save *saveFile) {
    if (EditorConfig.DrawingSubmenu)
        return;

    EditorConfig.DrawingSubmenu = true;

    while (aptMainLoop())
    {
        checkIfCardInserted();

        hidScanInput();
        Draw_PlayerMenu_Patterns(saveFile, EditorConfig.SelectedPlayer);
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
        }
    }

    EditorConfig.DrawingSubmenu = false;
}