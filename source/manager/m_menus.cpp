#include <3ds.h>
#include <string>
#include <algorithm>
#include "save.h"
#include "InputManager.h"
#include "utils.h"
#include "core/textbox.h"
#include "core/imagebutton.h"
#include "menus.h"
#include "manager/manager.h"

//static int g_selectedplayer = 0;
//static bool drawingMenu = false;
//static bool drawingSubmenu = false;

int spawn_manager_main_menu(Save *saveFile)
{
    while (aptMainLoop())
    {
        checkIfCardInserted();

        draw_manager_main_menu();
        InputManager::Instance()->RefreshInput();

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