#include <3ds.h>
#include <string>
#include <algorithm>
#include "CTRFont.hpp"
#include "InputManager.h"
#include "gfx.h"
#include "utils.h"
#include "config.hpp"
#include "core/nfs.h"
#include "core/keyboard.h"
#include "core/textbox.h"
#include "core/imagebutton.h"
#include "manager/manager.h"
#include "editor/editor.h"
#include "core/gui/ConfigMenu.hpp"
#include "core/gui/AboutMenu.hpp"
#include "menus.h"

extern FS_MediaType currentMediaType;

void checkIfCardInserted() {
    if (currentMediaType == MEDIATYPE_GAME_CARD && !FS::IsGameCartInserted()) {
        DisplayCardError();
    }
}

ReturnMode spawn_main_menu(void) {
    static ReturnMode ret = ReturnMode::None;

    while (aptMainLoop()) {
        draw_main_menu();
        InputManager::Instance()->RefreshInput();

        if (InputManager::Instance()->IsButtonActive(KEY_START)) {
            return ReturnMode::Exit;
        }

        //TODO: Needs to be reworked into using ImageButtons
        static const Rect_t editoract = {{90, 90}, {140, 140}};
        static const Rect_t manageract = {{180, 90}, {230, 140}};
        static const Rect_t aboutact = {{20, 30}, {100, 66}};
        static const Rect_t configact = {{220, 30}, {300, 66}};
        if (InputManager::Instance()->IsActive(editoract)) //Editor Icon
            ret = Editor::Main();

        else if (InputManager::Instance()->IsActive(manageract) && Config::Instance()->IsDebug) //Manager Icon
            ret = manager_main();

        else if (InputManager::Instance()->IsActive(aboutact)) //About Menu
            Core::Spawn_AboutMenu();

        else if (InputManager::Instance()->IsActive(configact)) //Config Menu
            Core::Spawn_ConfigMenu();

        if (ret == ReturnMode::Exit) {
            break;
        }

        else if (ret == ReturnMode::Editor) {
            ret = Editor::Main();
        }

        else if (ret == ReturnMode::Manager) {
            ret = manager_main();
        }
    }
    return ReturnMode::Exit;
}