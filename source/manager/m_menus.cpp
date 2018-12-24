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

ReturnMode spawn_manager_main_menu()
{
    while (aptMainLoop())
    {
        checkIfCardInserted();

        draw_manager_main_menu();
        InputManager::Instance()->RefreshInput();

        if (InputManager::Instance()->IsButtonDown(KEY_START))
        {
            return ReturnMode::Exit;
        }

        else if (InputManager::Instance()->IsButtonDown(KEY_B))
        {
            return ReturnMode::MainMenu;
        }
    }
    return ReturnMode::Exit;
}