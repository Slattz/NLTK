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
#include "menus.h"

extern FS_MediaType currentMediaType;
extern NLTK_Titles_Info MediaInfo;

static bool drawingMenu = false;

void checkIfCardInserted() {
    if (currentMediaType == MEDIATYPE_GAME_CARD && !IsGameCartInserted()) {
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
            spawn_about_menu();

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

u64 spawn_game_select_menu(FS_MediaType *mediaType)
{
    int selectedgame = -1;
    int selectedregion = -1;
    int selectedmedia = -1;

    checkInstalledTitles();

    if (MediaInfo.GameCartInfo.HasACNLData == false && MediaInfo.SDCardInfo.HasACNLData == false)
    {
        MsgDisp(top, "Error:\nACNL could not be found!");
        return 0;
    }

    while (aptMainLoop())
    {
        if (selectedmedia == 1 && !IsGameCartInserted()) {
            DisplayCardError();
        }

        draw_game_select_menu(selectedgame, selectedregion, selectedmedia);
        InputManager::Instance()->RefreshInput();

        if (InputManager::Instance()->IsButtonDown(KEY_START))
            break;

        NLTK_Media_Installed mediaInstalled = selectedmedia == 0
            ? MediaInfo.SDCardInfo : MediaInfo.GameCartInfo;

        static const Rect_t gamecart = {{101, 6}, {145, 54}};
        static const Rect_t sdcard = {{169, 6}, {213, 54}};
        static const Rect_t orig = {{101, 60}, {145, 104}};
        static const Rect_t WA = {{169, 60}, {213, 104}};
        static const Rect_t JPN = {{170, 130}, {214, 160}};
        static const Rect_t USA = {{106, 130}, {150, 160}};
        static const Rect_t EUR = {{42, 130}, {86, 160}};
        static const Rect_t KOR = {{234, 130}, {278, 160}};
        static const Rect_t confirm = {{75, 190}, {245, 220}};

        if (InputManager::Instance()->IsActive(gamecart) && MediaInfo.GameCartInfo.HasACNLData)
        {
            selectedmedia = 1;
            selectedgame = -1;
            selectedregion = -1;
        }
        else if (InputManager::Instance()->IsActive(sdcard) && MediaInfo.SDCardInfo.HasACNLData)
        {
            selectedmedia = 0;
            selectedgame = -1;
            selectedregion = -1;
        }
        else if (selectedmedia != -1 && InputManager::Instance()->IsActive(orig) && mediaInstalled.InstalledTitles.ORIG_installed)
        {
            selectedgame = 1;    //Orig ACNL
            selectedregion = -1; //Reset region, user mightn't have same regions installed for WA ACNL
        }
        else if (selectedmedia != -1 && InputManager::Instance()->IsActive(WA) && mediaInstalled.InstalledTitles.WA_installed)
        {
            selectedgame = 2;    //WA ACNL
            selectedregion = -1; //Reset region, user may not have same regions installed for orig ACNL
        }

        else if (InputManager::Instance()->IsActive(JPN))
        {
            if (selectedgame == 1 && mediaInstalled.InstalledTitles.ORIG_JPN_installed) //if orig ACNL
                selectedregion = 0;                                                     //JPN

            else if (selectedgame == 2 && mediaInstalled.InstalledTitles.WA_JPN_installed) //if WA ACNL
                selectedregion = 0;                                                        //JPN
        }

        else if (InputManager::Instance()->IsActive(USA))
        {
            if (selectedgame == 1 && mediaInstalled.InstalledTitles.ORIG_USA_installed) //if orig ACNL
                selectedregion = 1;                                                     //USA

            else if (selectedgame == 2 && mediaInstalled.InstalledTitles.WA_USA_installed) //if WA ACNL
                selectedregion = 1;                                                        //USA
        }
        else if (InputManager::Instance()->IsActive(EUR))
        {
            if (selectedgame == 1 && mediaInstalled.InstalledTitles.ORIG_EUR_installed) //if orig ACNL
                selectedregion = 2;                                                     //EUR

            else if (selectedgame == 2 && mediaInstalled.InstalledTitles.WA_EUR_installed) //if WA ACNL
                selectedregion = 2;                                                        //EUR
        }
        else if (InputManager::Instance()->IsActive(KOR))
        {
            if (selectedgame == 1 && mediaInstalled.InstalledTitles.ORIG_KOR_installed) //if orig ACNL
                selectedregion = 3;                                                     //KOR

            else if (selectedgame == 2 && mediaInstalled.InstalledTitles.WA_KOR_installed) //if WA ACNL
                selectedregion = 3;                                                        //KOR
        }

        else if (InputManager::Instance()->IsActive(confirm)) //Confirm Button
        {
            if (selectedmedia == -1)
                MsgDisp(top, "Please select a media type!");

            else if (selectedgame == -1)
                MsgDisp(top, "Please select a game!");

            else if (selectedregion == -1)
                MsgDisp(top, "Please select a region!");

            else
            {
                // Set Media Type
                *mediaType = static_cast<FS_MediaType>(selectedmedia + 1);

                if (selectedgame == 1)
                    return JPN_TID + (selectedregion * 0x100); //Form orig TID

                else if (selectedgame == 2)
                    return JPN_WA_TID + (selectedregion * 0x100); //Form WA TID

                else
                {
                    MsgDisp(top, Format("Error:\nSelected Media Type is: %d\nSelected Game is: %d\nSelected Region is: %d",
                                        selectedmedia, selectedgame, selectedregion));
                    return 0;
                }
            }
        }
    }
    return 0;
}

void spawn_about_menu(void)
{
    if (drawingMenu)
        return;

    drawingMenu = true;

    static bool discord = false;
    static bool twitter = false;
    while (aptMainLoop())
    {
        checkIfCardInserted();

        draw_about_menu(discord, twitter);
        InputManager::Instance()->RefreshInput();

        if (InputManager::Instance()->IsButtonDown(KEY_B))
            break;

        static const Rect_t discordrect = {{55, 180}, {105, 230}};
        static const Rect_t twiterrect = {{215, 180}, {265, 230}};

        if (InputManager::Instance()->IsActive(discordrect)) //Left Icon - Discord
            discord = true;

        else
            discord = false;

        if (InputManager::Instance()->IsActive(twiterrect)) //Right Icon - Twitter
            twitter = true;

        else
            twitter = false;
    }

    drawingMenu = false;
}
