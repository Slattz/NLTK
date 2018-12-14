#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "CTRFont.hpp"
#include "gfx.h"
#include "save.h"
#include "item.h"
#include "e_utils.h"
#include "utils.h"
#include "InputManager.h"
#include "menus.h"
#include "gui/PlayerMenu.hpp"

static std::vector<std::pair<std::string, s32>> inventoryData; // TODO: I dislike this. Find someother way of doing. Perhaps an item container class?

static void Draw_PlayerMenu_Inventory(int selectedplayer)
{
    int x = 42;
    int y = 63;

    Editor::Player::Draw_PlayerMenuTop(selectedplayer);
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

        Item item = Save::Instance()->players[selectedplayer].Pockets[i];
        DrawSprite(Common_ss, ITEM_HOLE, x - 16, y - 16);

        if (inventoryData[i].second > -1)
        {
            DrawSprite(Items_ss, inventoryData[i].second, x, y);
        }

        x += 38;
    }

    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

void Editor::Player::Spawn_PlayerMenu_Inventory() {
    if (EditorConfig.DrawingSubmenu)
        return;

    EditorConfig.DrawingSubmenu = true;

    inventoryData = load_player_invitems(EditorConfig.SelectedPlayer);

    while (aptMainLoop())
    {
        checkIfCardInserted();

        Draw_PlayerMenu_Inventory(EditorConfig.SelectedPlayer);
        InputManager::Instance()->RefreshInput();

        EditorConfig.LColor = EditorConfig.RColor = COLOR_GREY;

        if (InputManager::Instance()->IsButtonDown(KEY_B))
            break;

        if (InputManager::Instance()->IsButtonDown(KEY_R))
        {   
            EditorConfig.RColor = COLOR_WHITE;

            while (true) {
                EditorConfig.SelectedPlayer++;
                if (EditorConfig.SelectedPlayer > 3)
                    EditorConfig.SelectedPlayer = 0;

                if (Save::Instance()->players[EditorConfig.SelectedPlayer].Exists()) {
                    break;
                }
            }

            load_player_invitems(EditorConfig.SelectedPlayer);
        }

        if (InputManager::Instance()->IsButtonDown(KEY_L))
        {    
            EditorConfig.LColor = COLOR_WHITE;

            while (true) {
                EditorConfig.SelectedPlayer--;
                if (EditorConfig.SelectedPlayer < 0)
                    EditorConfig.SelectedPlayer = 3;

                if (Save::Instance()->players[EditorConfig.SelectedPlayer].Exists()) {
                    break;
                }
            }

            load_player_invitems(EditorConfig.SelectedPlayer);
        }
    }

    EditorConfig.DrawingSubmenu = false;
}