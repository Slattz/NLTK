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
#include "gui/ShopsMenu.hpp"

ShopsSettings ShopsConfig;

void Editor::Draw_ShopsMenu(Save *saveFile)
{
    draw_base_interface();
    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

void Editor::Spawn_ShopsMenu(Save *saveFile)
{
    if (ShopsConfig.DrawingMenu)
        return;

    ShopsConfig.DrawingMenu = true;

    while (aptMainLoop())
    {
        checkIfCardInserted();

        InputManager::Instance()->RefreshInput();
        Editor::Draw_ShopsMenu(saveFile);

        if (InputManager::Instance()->IsButtonDown(KEY_B))
            break;
    }

    ShopsConfig.DrawingMenu = false;
}