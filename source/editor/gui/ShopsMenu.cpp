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
#include "cursor.h"
#include "menus.h"
#include "gui/ShopsMenu.hpp"

extern Cursor g_cursor;
extern s16  g_CheckX[2];
extern s16  g_CheckY[2];
extern bool g_disabled[2];
extern u32  g_key[2];

ShopsSettings ShopsConfig;

void Editor::Draw_ShopsMenu(Save *saveFile)
{
    draw_base_interface();
    g_cursor.Draw();
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

        hidScanInput();
        Editor::Draw_ShopsMenu(saveFile);
        updateCursorInfo();

        if (hidKeysDown() & KEY_B)
            break;
    }

    ShopsConfig.DrawingMenu = false;
}