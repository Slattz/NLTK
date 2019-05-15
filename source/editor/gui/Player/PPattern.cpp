#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "CTRFont.hpp"
#include "gfx.h"
#include "save.h"
#include "pattern.h"
#include "e_utils.h"
#include "utils.h"
#include "InputManager.h"
#include "menus.h"
#include "gui/PlayerMenu.hpp"

static void Draw_PlayerMenu_Patterns(void)
{
    Editor::Player::Draw_PlayerMenuTop();
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

        C2D_DrawImageAt(Save::Instance()->players[PlayerConfig.SelectedPlayer]->Patterns[i]->Images[0], x, y, 0.5f, nullptr, 1.0f, 1.0f);
    }

    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

void Editor::Player::Spawn_PlayerMenu_Patterns() {
    if (PlayerConfig.DrawingSubmenu)
        return;

    PlayerConfig.DrawingSubmenu = true;

    while (aptMainLoop())
    {
        checkIfCardInserted();

        Draw_PlayerMenu_Patterns();
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

                if (Save::Instance()->players[PlayerConfig.SelectedPlayer]->Exists()) {
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

                if (Save::Instance()->players[PlayerConfig.SelectedPlayer]->Exists()) {
                    break;
                }
            }
        }
    }

    PlayerConfig.DrawingSubmenu = false;
}