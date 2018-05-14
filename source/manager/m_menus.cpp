#include "common.h"
#include "manager/manager.h"
#include "core/textbox.h"
#include "core/imagebutton.h"
#include <algorithm>

static int g_selectedplayer = 0;
static bool drawingMenu = false;
static bool drawingSubmenu = false;

int spawn_manager_main_menu(Save *saveFile)
{
    while (aptMainLoop())
    {
        checkIfCardInserted();

        hidScanInput();
        draw_manager_main_menu();
        updateCursorInfo();

        if (hidKeysDown() & KEY_START)
        {
            return MODE_EXIT;
        }

        else if (hidKeysDown() & KEY_B)
        {
            return MODE_MAINMENU;
        }
    }
    return 0;
}