#include "common.h"
#include "editor/editor.h"
#include "core/textbox.h"
#include "core/imagebutton.h"
#include <algorithm>

static int g_selectedplayer = 0;
static bool drawingMenu = false;
static bool drawingSubmenu = false;
s32 currentAcreId = -1;
ImageButton *currentAcreButton;

bool check_if_saving() {
	return confirmDisp(top, "Would you like to save changes\nmade to your save?");
}

int spawn_editor_main_menu(Save *saveFile)
{
    while (aptMainLoop())
    {
		checkIfCardInserted();

        hidScanInput();
        draw_editor_main_menu();
        updateCursorInfo();

        if (hidKeysDown() & KEY_START)
        {
            return MODE_EXIT;
        }

        else if (hidKeysDown() & KEY_B)
        {
            return MODE_MAINMENU;
        }

        for (int i = 0; i < 2; i++)
        {
            if (hidKeysDown() & g_key[i] && g_disabled[i])
            { 
                /* Menu Buttons */
                if (g_CheckX[i] >= 15 && g_CheckX[i] <= 85 && g_CheckY[i] >= 110 && g_CheckY[i] <= 165) //Town Menu
                    infoDisp(top, "Town Menu Coming Soon!");
    
				else if (g_CheckX[i] >= 89 && g_CheckX[i] <= 159 && g_CheckY[i] >= 110 && g_CheckY[i] <= 165) { //Acres Menu
					//infoDisp(top, "Acres Menu Coming Soon!");
					spawn_acres_menu(saveFile);
				}
                else if (g_CheckX[i] >= 163 && g_CheckX[i] <= 233 && g_CheckY[i] >= 110 && g_CheckY[i] <= 165) //Player Menu
                    spawn_player_menu(saveFile);
    
                else if (g_CheckX[i] >= 237 && g_CheckX[i] <= 307 && g_CheckY[i] >= 110 && g_CheckY[i] <= 165) //Villagers Menu
                    infoDisp(top, "Villagers Menu Coming Soon!");
    
				else if (g_CheckX[i] >= 15 && g_CheckX[i] <= 85 && g_CheckY[i] >= 180 && g_CheckY[i] <= 235) //Pwps Menu
                    infoDisp(top, "PWP Menu Coming Soon!");
    
				else if (g_CheckX[i] >= 89 && g_CheckX[i] <= 159 && g_CheckY[i] >= 180 && g_CheckY[i] <= 235) //Island Menu
                    infoDisp(top, "Island Menu Coming Soon!");
    
				else if (g_CheckX[i] >= 163 && g_CheckX[i] <= 233 && g_CheckY[i] >= 180 && g_CheckY[i] <= 235) //Main Street Menu
                    infoDisp(top, "Main Street Menu Coming Soon!");
    
				else if (g_CheckX[i] >= 237 && g_CheckX[i] <= 307 && g_CheckY[i] >= 180 && g_CheckY[i] <= 235) //About Menu
                    spawn_about_menu();
    
                /* Menu Columns */
				else if (g_CheckX[i] >= 220 && g_CheckX[i] <= 300 && g_CheckY[i] >= 10 && g_CheckY[i] <= 44) { //Save Column  
					if (check_if_saving()) {
						if (saveFile->Commit(false)) {
							infoDisp(top, "Save Succeeded!");
						}
						else {
							infoDisp(top, "Save Failed!");
						}
					}
				}
				else if (g_CheckX[i] >= 220 && g_CheckX[i] <= 300 && g_CheckY[i] >= 60 && g_CheckY[i] <= 99) //Options Column  
                    spawn_config_menu();
    
				else if (g_CheckX[i] >= 20 && g_CheckX[i] <= 100 && g_CheckY[i] >= 10 && g_CheckY[i] <= 44) { //Game Select Column  
                    return MODE_GAMESELECT;
                }

				else if (g_CheckX[i] >= 20 && g_CheckX[i] <= 100 && g_CheckY[i] >= 60 && g_CheckY[i] <= 99) //Town Manager Column  
                    return MODE_MANAGER;
            }
        }
    }
    return 0;
}

void spawn_player_menu(Save *saveFile)
{
	if (drawingMenu)
		return;

	drawingMenu = true;

    static int selectedmode = 0;
    static u32 LColor, RColor;

	for (int i = 0; i < 4; ++i) {
		u8 *TPCData = saveFile->players[i].m_TPCData;
		bool loadedTPCImage = false;

		if (TPCData != nullptr) {
			loadedTPCImage = load_texture_jpeg_memory(PLAYER1_PIC + i, TPCData, 0x1400, 64, 104);
			continue;
		}

		// If loadedTPCImage is false, either the TPC didn't exist or it was invalid. Fall back to the NOTPC for now.
		if (!loadedTPCImage) {
			pp2d_load_texture_png(PLAYER1_PIC + i, "romfs:/Menus/Players/NoTPC.png"); // This was still triggering before, so the continue above is needed.
		}
	}

    while (aptMainLoop())
    {
		checkIfCardInserted();

        hidScanInput();
        draw_player_menu(saveFile, g_selectedplayer, selectedmode, LColor, RColor);
        updateCursorInfo();

        LColor = RColor = COLOR_GREY;

        if (hidKeysDown() & KEY_B)
            break;

        if (hidKeysHeld() & KEY_R)
            RColor = COLOR_WHITE;

        if (hidKeysHeld() & KEY_L)
            LColor = COLOR_WHITE;

		if (hidKeysDown() & KEY_R)
		{
			while (true) {
				g_selectedplayer++;
				if (g_selectedplayer > 3)
					g_selectedplayer = 0;

				if (saveFile->players[g_selectedplayer].Exists(saveFile)) {
					break;
				}
			}
		}

		if (hidKeysDown() & KEY_L)
		{
			while (true) {
				g_selectedplayer--;
				if (g_selectedplayer < 0)
					g_selectedplayer = 3;

				if (saveFile->players[g_selectedplayer].Exists(saveFile)) {
					break;
				}
			}
		}

        for (int i = 0; i < 2; i++)
        {
            if (hidKeysDown() & g_key[i] && g_disabled[i])
            { 
                /* Menu Buttons */
				if (g_CheckX[i] >= 0 && g_CheckX[i] <= 58 && g_CheckY[i] >= 0 && g_CheckY[i] < 40) //Info
				{
					selectedmode = 0;
					spawn_player_menu_info(saveFile);
				}
                else if (g_CheckX[i] >= 0 && g_CheckX[i] <= 58 && g_CheckY[i] >= 40 && g_CheckY[i] < 80) //Inv
                {
                    selectedmode = 1;
                    spawn_player_menu_inventory(saveFile);
                }
				else if (g_CheckX[i] >= 0 && g_CheckX[i] <= 58 && g_CheckY[i] >= 80 && g_CheckY[i] < 120) //Appearance
				{
					selectedmode = 2;
				}
				else if (g_CheckX[i] >= 0 && g_CheckX[i] <= 58 && g_CheckY[i] >= 120 && g_CheckY[i] < 160) //House
				{
					selectedmode = 3;
				}
				else if (g_CheckX[i] >= 0 && g_CheckX[i] <= 58 && g_CheckY[i] >= 160 && g_CheckY[i] < 200) //Flags
				{
					selectedmode = 4;
				}
				else if (g_CheckX[i] >= 0 && g_CheckX[i] <= 58 && g_CheckY[i] >= 200 && g_CheckY[i] <= 240) //Mail
				{
					selectedmode = 5;
				}
            }
        }

/*        if (selectedmode == 0) //PLYR_ABOUT
            draw_player_menu_about(savebuf, selectedplayer);

        else if (selectedmode == 1) //PLYR_INVENTORY
            spawn_player_menu_inventory(savebuf, selectedplayer);
            
        else if (selectedmode == 2) //PLYR_APPEARANCE
            draw_player_menu_appearance(savebuf, selectedplayer);
            
        else if (selectedmode == 3) //PLYR_HOUSE
            draw_player_menu_house(savebuf, selectedplayer);
            
        else if (selectedmode == 4) //PLYR_PATTERN
            draw_player_menu_patterns(savebuf, selectedplayer);
            
        else if (selectedmode == 5) //PLYR_MAILBOX(_UNREAD)
            draw_player_menu_mailbox(savebuf, selectedplayer);*/
    }

	drawingMenu = false;
}

void refreshInfoMenu(Save *saveFile) {
	playerNameBox.Text = u16tou8(saveFile->players[g_selectedplayer].Name); // TODO: Change player name to utf8? Or at least create a utf8 equivalent
	playerWalletBox.Text = std::to_string(saveFile->players[g_selectedplayer].Wallet.value);
	playerSavingsBox.Text = std::to_string(saveFile->players[g_selectedplayer].Savings.value);
	playerMedalsBox.Text = std::to_string(saveFile->players[g_selectedplayer].IslandMedals.value);
	playerCouponsBox.Text = std::to_string(saveFile->players[g_selectedplayer].MeowCoupons.value);
}

void spawn_player_menu_info(Save *saveFile) {
	if (drawingSubmenu)
		return;

	drawingSubmenu = true;

	static u32 LColor, RColor;
	u32 m_keysDown, m_keysHeld = 0;

	// Set initial textbox values
	refreshInfoMenu(saveFile);

	while (aptMainLoop()) {
		checkIfCardInserted();

		hidScanInput();
        draw_player_menu_info(saveFile, g_selectedplayer, LColor, RColor);
		updateCursorInfo();

		m_keysDown = hidKeysDown();
		m_keysHeld = hidKeysHeld();

		LColor = RColor = COLOR_GREY;

		if (m_keysDown & KEY_B || m_keysHeld & KEY_B)
			break;

		if (m_keysHeld & KEY_R)
			RColor = COLOR_WHITE;

		if (m_keysHeld & KEY_L)
			LColor = COLOR_WHITE;

		if (m_keysDown & KEY_R)
		{
			while (true) {
				g_selectedplayer++;
				if (g_selectedplayer > 3)
					g_selectedplayer = 0;

				if (saveFile->players[g_selectedplayer].Exists(saveFile)) {
					break;
				}
			}

			refreshInfoMenu(saveFile);
		}

		if (m_keysDown & KEY_L)
		{
			while (true) {
				g_selectedplayer--;
				if (g_selectedplayer < 0)
					g_selectedplayer = 3;

				if (saveFile->players[g_selectedplayer].Exists(saveFile)) {
					break;
				}
			}

			refreshInfoMenu(saveFile);
		}

		for (u32 i = 0; i < 2; i++)
		{
			if ((m_keysDown & g_key[i]) && g_disabled[i])
			{
				// Check for input in the info menu
				if (playerNameBox.CheckActivate(g_CheckX[i], g_CheckY[i])) { // Player Name Box
					saveFile->players[g_selectedplayer].Name = u8tou16(playerNameBox.Activate().c_str());
					saveFile->SetChangesMade(true);
				}
				else if (playerWalletBox.CheckActivate(g_CheckX[i], g_CheckY[i])) {
					saveFile->players[g_selectedplayer].Wallet.value = static_cast<u32>(std::stoi(playerWalletBox.Activate()));
					saveFile->SetChangesMade(true);
				}
				else if (playerSavingsBox.CheckActivate(g_CheckX[i], g_CheckY[i])) {
					saveFile->players[g_selectedplayer].Savings.value = static_cast<u32>(std::stoi(playerSavingsBox.Activate()));
					saveFile->SetChangesMade(true);
				}
				else if (playerMedalsBox.CheckActivate(g_CheckX[i], g_CheckY[i])) {
					saveFile->players[g_selectedplayer].IslandMedals.value = static_cast<u32>(std::stoi(playerMedalsBox.Activate()));
					saveFile->SetChangesMade(true);
				}
				else if (playerCouponsBox.CheckActivate(g_CheckX[i], g_CheckY[i])) {
					saveFile->players[g_selectedplayer].MeowCoupons.value = static_cast<u32>(std::stoi(playerCouponsBox.Activate()));
					saveFile->SetChangesMade(true);
				}
			}
		}
	}

	drawingSubmenu = false;
}

void spawn_player_menu_inventory(Save *saveFile)
{
	if (drawingSubmenu)
		return;

	drawingSubmenu = true;

    static u32 LColor, RColor;
    load_player_invitems(saveFile, g_selectedplayer);
    
    while (aptMainLoop())
    {
		checkIfCardInserted();

        hidScanInput();
        draw_player_menu_inventory(saveFile, g_selectedplayer, LColor, RColor);
        updateCursorInfo();

        LColor = RColor = COLOR_GREY;

        if (hidKeysDown() & KEY_B)
            break;

        if (hidKeysHeld() & KEY_R)
            RColor = COLOR_WHITE;

        if (hidKeysHeld() & KEY_L)
            LColor = COLOR_WHITE;

        if (hidKeysDown() & KEY_R)
        {    
			while (true) {
				g_selectedplayer++;
				if (g_selectedplayer > 3)
					g_selectedplayer = 0;

				if (saveFile->players[g_selectedplayer].Exists(saveFile)) {
					break;
				}
			}

            load_player_invitems(saveFile, g_selectedplayer);
        }

        if (hidKeysDown() & KEY_L)
        {    
			while (true) {
				g_selectedplayer--;
				if (g_selectedplayer < 0)
					g_selectedplayer = 3;

				if (saveFile->players[g_selectedplayer].Exists(saveFile)) {
					break;
				}
			}

            load_player_invitems(saveFile, g_selectedplayer);
        }
    }

	drawingSubmenu = false;
}

void view_acres_town_full(Save *saveFile)
{
    InitAcreGFX(saveFile, 42, 7, 20, 0, 0, MAP_ACRES); // Ends at x = 230px (resized to 30x30 images)
}

void view_acres_island_full(Save *saveFile)
{
	InitAcreGFX(saveFile, 16, 4, 80, 40, 0, ISLAND_ACRES);
}

void view_acres_town_map(Save *saveFile)
{
	InitAcreGFX(saveFile, 20, 5, 60, 40, 4, MAP_ACRES + 0x10);
}

void view_acres_island_map(Save *saveFile)
{
	InitAcreGFX(saveFile, 4, 2, 120, 80, 4, ISLAND_ACRES + 0xA);
}

// Acre Menu
void spawn_acres_menu(Save *saveFile)
{
	if (drawingMenu)
		return;

	drawingMenu = true;
	acreEditorSelectedAcre = -1;
	currentAcreId = -1;
	currentAcreButton = nullptr;

	// Init Acre Graphics
	view_acres_town_full(saveFile);

	while (aptMainLoop())
	{
		checkIfCardInserted();

		hidScanInput();
		
		if (true) {
			draw_save_acres(saveFile);
		}

		updateCursorInfo();

		if (hidKeysDown() & KEY_B) {
			break;
		}

		if (acreEditorSelectedAcre > 0 && hidKeysDown() & KEY_DLEFT) {
			acreEditorSelectedAcre--;
		}
		else if (acreEditorSelectedAcre > -1 && acreEditorSelectedAcre < ACRE_ID_MAX && hidKeysDown() & KEY_DRIGHT) {
			acreEditorSelectedAcre++;
		}

		if (acreEditorSelectedAcre > -1 && acreEditorSelectedAcre <= ACRE_ID_MAX && hidKeysDown() & KEY_A) {
			currentAcreButton->SetImageId(ACRE_PNG + acreEditorSelectedAcre);
			u32 writeOffset = MAP_ACRES
				+ std::distance(acreEditorControls.begin(), std::find(acreEditorControls.begin(), acreEditorControls.end(), currentAcreButton)) * 2;
			saveFile->Write(writeOffset, static_cast<u8>(acreEditorSelectedAcre));

			// Clear selected acre
			acreEditorSelectedAcre = -1;
			currentAcreButton->SetActive(false);
			currentAcreButton = nullptr;
			currentAcreId = -1;
		}

		for (int i = 0; i < 2; i++)
		{
			if (hidKeysDown() & g_key[i] && g_disabled[i])
			{
				for (Control *c : acreEditorControls) {
					auto imageButton = reinterpret_cast<ImageButton*>(c);
					if (imageButton && imageButton->CheckActivate(g_CheckX[i], g_CheckY[i])) { // cast was successful?
						imageButton->Activate();
						break;
					}
				}
			}
		}
	}

	drawingMenu = false;
}

void onAcreClick(Button *sender) {
	if (sender != nullptr) {

		// Disable all buttons
		if (currentAcreButton != nullptr) {
			for (Control *c : acreEditorControls) {
				reinterpret_cast<ImageButton *>(c)->SetActive(false);
			}
		}

		sender->SetActive(true);
		currentAcreButton = reinterpret_cast<ImageButton *>(sender);
		currentAcreId = currentAcreButton->GetImageId() - ACRE_PNG;
		acreEditorSelectedAcre = currentAcreId;
	}
}