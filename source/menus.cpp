#include "common.h"
#include "core/textbox.h"
#include "core/imagebutton.h"
#include "manager/manager.h"
#include "editor/editor.h"
#include <algorithm>

s16  g_CheckX[2];
s16  g_CheckY[2];
bool g_disabled[2];
u32  g_key[2] = {KEY_TOUCH, KEY_A};
static bool drawingMenu = false;

void checkIfCardInserted() {
    if (currentMediaType == MEDIATYPE_GAME_CARD && !IsGameCartInserted()) {
        DisplayCardError();
    }
}

int spawn_main_menu(void) {
    static int ret = -1;

    while (aptMainLoop()) {
		checkIfCardInserted();

        hidScanInput();
        draw_main_menu();
        updateCursorInfo();

        if (hidKeysDown() & KEY_START) {
            return MODE_EXIT;
        }

        for (int i = 0; i < 2; i++)
        {
            if (hidKeysDown() & g_key[i] && g_disabled[i])
            { 
				if (g_CheckX[i] >= 60 && g_CheckX[i] <= 124 && g_CheckY[i] >= 50 && g_CheckY[i] <= 114) //Editor Icon
                    ret = editor_main();
    
				else if (g_CheckX[i] >= 180 && g_CheckX[i] <= 244 && g_CheckY[i] >= 50 && g_CheckY[i] <= 114) //Manager Icon
                    ret = manager_main();

                else if (g_CheckX[i] >= 20 && g_CheckX[i] <= 100 && g_CheckY[i] >= 30 && g_CheckY[i] <= 66) //About Menu
                    spawn_about_menu();

                else if (g_CheckX[i] >= 220 && g_CheckX[i] <= 300 && g_CheckY[i] >= 30 && g_CheckY[i] <= 66) //Config Menu
                    spawn_config_menu();
            }
        }

        if (ret == MODE_EXIT) {
            break;
        }

        else if (ret == MODE_EDITOR) {
            ret = editor_main();
        }

        else if (ret == MODE_MANAGER) {
            ret = manager_main();
        }
    }
    return 0;
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

        hidScanInput();
        draw_game_select_menu(selectedgame, selectedregion, selectedmedia);
        updateCursorInfo();

        if (hidKeysDown() & KEY_START)
            break;

		NLTK_Media_Installed mediaInstalled = selectedmedia == 0
			? MediaInfo.SDCardInfo : MediaInfo.GameCartInfo;

        for (int i = 0; i < 2; i++)
        {
            if (hidKeysDown() & g_key[i] && g_disabled[i])
            { 
				if (g_CheckX[i] >= 101 && g_CheckX[i] <= 145 && g_CheckY[i] >= 6 && g_CheckY[i] <= 54 && MediaInfo.GameCartInfo.HasACNLData) {
					selectedmedia = 1;
					selectedgame = -1;
					selectedregion = -1;
				}
				else if (g_CheckX[i] >= 169 && g_CheckX[i] <= 213 && g_CheckY[i] >= 6 && g_CheckY[i] <= 54 && MediaInfo.SDCardInfo.HasACNLData) {
					selectedmedia = 0;
					selectedgame = -1;
					selectedregion = -1;
				}
                else if (selectedmedia != -1 && g_CheckX[i] >= 101 && g_CheckX[i] <= 145 && g_CheckY[i] >= 60 && g_CheckY[i] <= 104
                    && mediaInstalled.InstalledTitles.ORIG_installed)
                {
                    selectedgame = 1; //Orig ACNL
                    selectedregion = -1; //Reset region, user mightn't have same regions installed for WA ACNL
                }
                else if (selectedmedia != -1 && g_CheckX[i] >= 169 && g_CheckX[i] <= 213 && g_CheckY[i] >= 60 && g_CheckY[i] <= 104
                    && mediaInstalled.InstalledTitles.WA_installed)
                {
                    selectedgame = 2; //WA ACNL
                    selectedregion = -1; //Reset region, user may not have same regions installed for orig ACNL
                }
				else if (g_CheckX[i] >= 170 && g_CheckX[i] <= 214 && g_CheckY[i] >= 130 && g_CheckY[i] <= 160)
                {
                    if (selectedgame == 1 && mediaInstalled.InstalledTitles.ORIG_JPN_installed) //orig ACNL
                        selectedregion = 0; //JPN
    
                    else if (selectedgame == 2 && mediaInstalled.InstalledTitles.WA_JPN_installed) //WA ACNL
                        selectedregion = 0; //JPN
                }
				else if (g_CheckX[i] >= 106 && g_CheckX[i] <= 150 && g_CheckY[i] >= 130 && g_CheckY[i] <= 160)
                {
                    if (selectedgame == 1 && mediaInstalled.InstalledTitles.ORIG_USA_installed) //orig ACNL
                        selectedregion = 1; //USA
    
                    else if (selectedgame == 2 && mediaInstalled.InstalledTitles.WA_USA_installed) //WA ACNL
                        selectedregion = 1; //USA
                }
				else if (g_CheckX[i] >= 42 && g_CheckX[i] <= 86 && g_CheckY[i] >= 130 && g_CheckY[i] <= 160)
                {
                    if (selectedgame == 1 && mediaInstalled.InstalledTitles.ORIG_EUR_installed) //orig ACNL
                        selectedregion = 2; //EUR
    
                    else if (selectedgame == 2 && mediaInstalled.InstalledTitles.WA_EUR_installed) //WA ACNL
                        selectedregion = 2; //EUR
                }
                else if (g_CheckX[i] >= 234 && g_CheckX[i] <= 278 && g_CheckY[i] >= 130 && g_CheckY[i] <= 160)
                {
                    if (selectedgame == 1 && mediaInstalled.InstalledTitles.ORIG_KOR_installed) //orig ACNL
                        selectedregion = 3; //KOR
    
                    else if (selectedgame == 2 && mediaInstalled.InstalledTitles.WA_KOR_installed) //WA ACNL
                        selectedregion = 3; //KOR
                }
				else if (g_CheckX[i] >= 75 && g_CheckX[i] <= 245 && g_CheckY[i] >= 190 && g_CheckY[i] <= 220) //Confirm Button
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

        hidScanInput();
        draw_about_menu(discord, twitter);
        updateCursorInfo();
        
        if (hidKeysDown() & KEY_B)
            break;
        int i;
        for (i = 0; i < 2; i++)
        {
            if (g_disabled[i])
            { 
                if (g_CheckX[i] >= 55 && g_CheckX[i] <= 105 && g_CheckY[i] >= 180 && g_CheckY[i] <= 230) //Left Icon - Discord
                    discord = true;

                else discord = false;
                /*
                if (g_CheckX[i] >= 135 && g_CheckX[i] <= 185 && g_CheckY[i] >= 180 && g_CheckY[i] <= 230) //Middle Icon - N/A
                    idk = true;

                else idk = false;
                */
                if (g_CheckX[i] >= 215 && g_CheckX[i] <= 265 && g_CheckY[i] >= 180 && g_CheckY[i] <= 230) //Right Icon - Twitter
                    twitter = true;

                else twitter = false;
            }
        }
    }

	drawingMenu = false;
}

void spawn_config_menu(void)
{
	if (drawingMenu)
		return;

	drawingMenu = true;

    static bool debugcomplete = false;

    while (aptMainLoop())
    {
		checkIfCardInserted();

        hidScanInput();
        draw_config_menu();
        updateCursorInfo();

        if (DebugCode(debugcomplete))
        {
            debugcomplete = true;
            config.isdebug = true;
        }
        
        if (hidKeysDown() & KEY_B)
            break;

        for (int i = 0; i < 2; i++)
        {
            if (hidKeysDown() & g_key[i] && g_disabled[i])
            { 
                if (g_CheckX[i] >= 20 && g_CheckX[i] <= 44 && g_CheckY[i] >= 20 && g_CheckY[i] <= 44) //config.autoupdate
                    config.autoupdate = !config.autoupdate;
    
                if (g_CheckX[i] >= 20 && g_CheckX[i] <= 44 && g_CheckY[i] >= 48  && g_CheckY[i] <= 72) //config.autosavebackup
                    config.autosavebackup = !config.autosavebackup;
    
                if (g_CheckX[i] >= 20 && g_CheckX[i] <= 44 && g_CheckY[i] >= 76 && g_CheckY[i] <= 100) //config.autoloadprefGame
                    config.autoloadprefGame = !config.autoloadprefGame;
    
                if (g_CheckX[i] >= 20 && g_CheckX[i] <= 44 && g_CheckY[i] >= 104 && g_CheckY[i] <= 128) //config.isdebug
                    if (config.isdebug)
                    {
                        config.isdebug = false;
                        debugcomplete = false;
                    }
            }
        }
    }
    saveConfig();
	drawingMenu = false;
}