#include "gfx.h"
#include "about.h"

C3D_RenderTarget* top;
C3D_RenderTarget* bottom;
C2D_ImageTint* GreyFilter = new C2D_ImageTint[sizeof(C2D_ImageTint)];
C2D_ImageTint* GreenFilter = new C2D_ImageTint[sizeof(C2D_ImageTint)];

C2D_SpriteSheet About_ss;
C2D_SpriteSheet Acres_ss;
C2D_SpriteSheet Common_ss;
C2D_SpriteSheet GameSelect_ss;
C2D_SpriteSheet Editor_ss;
C2D_SpriteSheet Items_ss;
C2D_SpriteSheet Players_ss;

void InitGFX(void)
{
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    /* Load Spritesheets */
    About_ss = C2D_SpriteSheetLoad("romfs:/gfx/About.t3x");
    Acres_ss = C2D_SpriteSheetLoad("romfs:/gfx/Acres.t3x");
    Common_ss = C2D_SpriteSheetLoad("romfs:/gfx/Common.t3x");
    GameSelect_ss = C2D_SpriteSheetLoad("romfs:/gfx/GameSelect.t3x");
    Editor_ss = C2D_SpriteSheetLoad("romfs:/gfx/Editor.t3x");
    Items_ss = C2D_SpriteSheetLoad("romfs:/gfx/Items.t3x");
    Players_ss = C2D_SpriteSheetLoad("romfs:/gfx/Players.t3x");

    /* Init Tints */
    C2D_PlainImageTint(GreyFilter, COLOR_GREY_FILTER, 1.0);
    C2D_PlainImageTint(GreenFilter, COLOR_GREEN, 1.0);
}

void ExitGFX(void)
{
    C2D_SpriteSheetFree(About_ss);
    C2D_SpriteSheetFree(Acres_ss);
    C2D_SpriteSheetFree(Common_ss);
    C2D_SpriteSheetFree(GameSelect_ss);
    C2D_SpriteSheetFree(Editor_ss);
    C2D_SpriteSheetFree(Items_ss);
    C2D_SpriteSheetFree(Players_ss);
    C2D_Fini();
    C3D_Fini();
    gfxExit();
}

void DrawSprite(C2D_SpriteSheet sheet, size_t imgindex, float x, float y, const C2D_ImageTint *tint, float scaleX, float scaleY)
{
    C2D_Image img;
    img = C2D_SpriteSheetGetImage(sheet, imgindex);
    C2D_DrawImageAt(img, x, y, 1.0, tint, scaleX, scaleY);
}

void DrawText(float x, float y, float scaleX, float scaleY, u32 color, const char* text, bool atBaseline)
{
    Text Txt(Color(color), text, scaleX, scaleY);
    Txt.Draw(x, y, atBaseline);
}

void draw_base_interface(void)
{
    static char title[50] = TITLE " " VERSION;
    int titleX = 315;
    if (BETA_VERSION > 0)
    {
        strcat(title, " B" TOSTRING(BETA_VERSION));
        titleX -= 15;
    }


    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, COLOR_BG);
    C2D_TargetClear(bottom, COLOR_BG);

    C2D_SceneBegin(top);
    C2D_DrawRectSolid(0, 0, 0, 400, 20, COLOR_MENU_BARS);
    DrawText(titleX, 2, 0.5, 0.5, COLOR_GREY, title);

    if (config.isdebug)
        DrawText(3, 2, 0.5, 0.5, COLOR_ORANGE, "Debug: ON");
    C2D_Flush();
}

void draw_cursor(void)
{
    C2D_SceneBegin(bottom);
    updatePointerPosition();

    if (g_cursorpos.show)
    {
        if (g_cursorpos.A_held)
            DrawSprite(Common_ss, CURSOR_SELECT, g_cursorpos.x, g_cursorpos.y);
        
        else
            DrawSprite(Common_ss, CURSOR_POINT, g_cursorpos.x, g_cursorpos.y);
    }
    C2D_Flush();
}

void infoDisp(C3D_RenderTarget *target, const char* message)
{
    char     temp[1024] = {0};
    strncpy(temp, message, 1023);
    strncat(temp, "\n\nPress " FONT_A " to Continue.", 40);

    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_A) break;
        
        draw_base_interface();
        C2D_SceneBegin(target);
        C2D_DrawRectSolid(50, 33.5, 0, 300, 180, COLOR_BROWN);
        draw_centered_text(120, 280, 80, 0, 0.7f, 0.7f, COLOR_GREY, temp);
        C3D_FrameEnd(0);
    }
}

void infoDispF(C3D_RenderTarget *target, const char* string, ...)
{
    char       buffer[1024];
    va_list    args;
    va_start(args, string);
    vsnprintf(buffer, sizeof(buffer), string, args);
    infoDisp(target, buffer);
    va_end(args);
}

bool confirmDisp(C3D_RenderTarget *target, const char* message)
{
    char     temp[1024] = {0};
    strncpy(temp, message, 1023);
    strncat(temp, "\n\nYes: " FONT_A ", No: " FONT_B, 40);

    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_A) return true;
        if (hidKeysDown() & KEY_B) return false;
        
        draw_base_interface();
        C2D_SceneBegin(target);
        C2D_DrawRectSolid(50, 33.5, 0, 300, 180, COLOR_BROWN);
        draw_centered_text(120, 280, 80, 0, 0.7f, 0.7f, COLOR_GREY, temp);
        C3D_FrameEnd(0);
    }
    return 0;
}

void confirmDispF(C3D_RenderTarget *target, const char* string, ...)
{
    char       buffer[1024];
    va_list    args;
    va_start(args, string);
    vsnprintf(buffer, sizeof(buffer), string, args);
    confirmDisp(target, buffer);
    va_end(args);
}

void DisplayText(C3D_RenderTarget *target, int x, int y, int width, int height, float textsize, const char* message)
{
	char     temp[1024] = { 0 };
	strncpy(temp, message, 1023);
	C2D_SceneBegin(target);
	C2D_DrawRectSolid(x, y, 0, width, height, COLOR_BROWN);
    draw_centered_text(x+5, 400-x-5, y+5, 0, textsize, textsize, COLOR_GREY, temp);
	//Caller function must call C3D_FrameEnd(0);
}

void DisplayTextF(C3D_RenderTarget *target, int x, int y, int width, int height, float textsize, const char* string, ...)
{
    char       buffer[1024];
    va_list    args;
    va_start(args, string);
    vsnprintf(buffer, sizeof(buffer), string, args);
    DisplayText(target, x, y, width, height, textsize, buffer);
    va_end(args);
}

void DisplayCardError() {
	// TODO: This shouldn't just check if the card is inserted, but rather, if Animal Crossing is the inserted card.
	if (IsGameCartInserted() && checkGameCartIsACNL()) {
		return;
	}

	bool show = true;
	u32 timer = 0;
	u32 maxTimer = 60; // 60 FPS?
	const char *cardRemovedErrorMessage = "To continue editing, please\nreinsert your Animal Crossing: New Leaf\nGame Card.\n" \
		"Press " FONT_A " when done.";

	while (aptMainLoop())
	{
		hidScanInput();

		if (IsGameCartInserted() && checkGameCartIsACNL() && hidKeysDown() & KEY_A) break;

		draw_base_interface();
		C2D_SceneBegin(top);
		C2D_DrawRectSolid(50, 33.5, 0, 300, 180, COLOR_BROWN);
		draw_centered_text(0, 400, 100, 0, 0.5, 0.5, COLOR_GREY, cardRemovedErrorMessage);

		if (show || IsGameCartInserted()) {
			DrawSprite(GameSelect_ss, GAME_CART, 176, 40); // NOTE: Top Screen resolution 400x240, Bottom Screen Resolution: 320x240
		}
		else {
			DrawSprite(GameSelect_ss, GAME_CART, 176, 40, GreyFilter);
		}

		C3D_FrameEnd(0);

		timer++;
		if (timer >= maxTimer) {
			timer = 0;
			show = !show;
		}
	}
}

void draw_main_menu(void)
{
    draw_base_interface();
    C2D_SceneBegin(bottom);
    DrawSprite(Common_ss, NLTK_ICON, 126, 10); //NLTK's Icon
    DrawSprite(Editor_ss, BUTTON_MAIN, 20,  30, nullptr, 1.15f, 0.6f);  //w = 80, h = 33
    DrawSprite(Editor_ss, BUTTON_MAIN, 220, 30, nullptr, 1.15f, 0.6f);  //w = 80, h = 33
    draw_centered_text(20,  80, 30, 33, 0.46, 0.46, COLOR_GREY, "About"); //Column 1 Text
    draw_centered_text(220, 80, 30, 33, 0.48, 0.48, COLOR_GREY, "Options"); //Column 2 Text
    

    DrawSprite(Common_ss, EDITOR_ICON, 60, 70); //Editor Icon
    DrawSprite(Common_ss, MANAGER_ICON, 180, 70); //Manager Icon
    DrawText(60, 130, 0.5, 0.5, COLOR_GREY, "Editor");
    DrawText(180, 130, 0.5, 0.5, COLOR_GREY, "Manager");
    
    C2D_SceneBegin(top);
    draw_centered_text(0, 400, 80, 0, 1.1, 1.1, COLOR_GREY, "Actual Main Menu!");

    if (config.isdebug)
    {
        DrawText(100, 120, 0.5, 0.5, COLOR_GREY, Format("Cursor X: %d, Cursor Y: %d", g_cursorpos.x, g_cursorpos.y).c_str());
    }
    draw_cursor();
    C3D_FrameEnd(0);
}

void draw_config_menu(void)
{
    const char* ConfigText[] = {"Auto Update", "Auto Save Backup", "Auto Load Preferred Game", "Debug"};
    bool  ConfigBools[] = {config.autoupdate, config.autosavebackup, config.autoloadprefGame, 
                    config.isdebug};
    u32 configmax = sizeof(ConfigBools)/sizeof(ConfigBools[0]);

    draw_base_interface();
    C2D_SceneBegin(bottom);

    u32 i;
    for (i = 0; i < configmax-1; i++) //configmax-1: exclude Debug
    {
        if (ConfigBools[i])
            DrawSprite(Common_ss, CHECKBOX_FILLED, 20, 20+(28*i));

        else
            DrawSprite(Common_ss, CHECKBOX_EMPTY, 20, 20+(28*i));

        DrawText(52, 21+(28*(configmax-1)), 0.6, 0.6, COLOR_GREY, ConfigText[configmax-1]); //ConfigText
    }

    if (config.isdebug)
    {
        DrawSprite(Common_ss, CHECKBOX_FILLED, 20, 20+(28*(configmax-1)));
        DrawText(52, 21+(28*(configmax-1)), 0.6, 0.6, COLOR_GREY, ConfigText[configmax-1]);
    }
    draw_cursor();
    C3D_FrameEnd(0);
}

void draw_about_menu(bool discord, bool twitter)
{
    draw_base_interface();
    DrawText(175, 30, 0.6, 0.6, COLOR_GREY, "Credits:");

    for (int i = 0; i < creditsCount; i++)
    {
        DrawText(10, 55+(i*20), 0.46, 0.46, COLOR_GREY, credits[i*2]); //Name
        DrawText(120, 55+(i*20), 0.45, 0.45, COLOR_GREY, credits[1+i*2]); //Description
    }
    C2D_SceneBegin(bottom);
    DrawSprite(Common_ss, NLTK_ICON, 126, 10); //NLTK's Icon
    DrawSprite(About_ss, DISCORD_ICON, 55, 180); //Discord Icon
    DrawSprite(About_ss, TWITTER_ICON, 215, 180); //Twitter Icon
    /* L:55, M:135, R:215 */

    if (discord)
        DisplayText(bottom, 60, 100, 200, 70, 0.5, discordtext);

    if (twitter)
        DisplayText(bottom, 60, 100, 200, 70, 0.5, twittertext);

    draw_cursor();
    C3D_FrameEnd(0);
}

void draw_game_select_menu(int selectedgame, int selectedregion, int selectedmedia)
{
    draw_base_interface();
    C2D_SceneBegin(bottom);

	if (selectedmedia == -1 || selectedgame == -1 || selectedregion == -1) {
		C2D_DrawRectSolid(75, 190, 0, 170, 30, COLOR_BROWN); //Confirm Button
	}
	else {
		C2D_DrawRectSolid(75, 190, 0, 170, 30, COLOR_LIGHT_BROWN); //Confirm Button (Can Confirm)
	}

	// Media Type
	if (selectedmedia == 1) { //Game Card
		C2D_DrawRectSolid(98, 3, 0, 54, 54, COLOR_GREY); //Grey Outline
		DrawSprite(GameSelect_ss, GAME_CART, 101, 6); //Game Card is selected
		DrawSprite(GameSelect_ss, SD_CARD, 169, 6, GreyFilter); //Grey filter over SD card
	}
	else if (selectedmedia == 0) { //SD Card
        C2D_DrawRectSolid(166, 3, 0, 54, 54, COLOR_GREY); //Grey Outline
        DrawSprite(GameSelect_ss, SD_CARD, 169, 6); //SD Card is selected
		DrawSprite(GameSelect_ss, GAME_CART, 101, 6, GreyFilter); //Grey Filter over Game Cart
	}
	else { //No Media selected yet
		if (MediaInfo.GameCartInfo.HasACNLData) {
			DrawSprite(GameSelect_ss, GAME_CART, 101, 6);
		}
		else {
			DrawSprite(GameSelect_ss, GAME_CART, 101, 6, GreyFilter);
		}

		if (MediaInfo.SDCardInfo.HasACNLData) {
			DrawSprite(GameSelect_ss, SD_CARD, 169, 6);
		}
		else {
			DrawSprite(GameSelect_ss, SD_CARD, 169, 6, GreyFilter);
		}
	}

	NLTK_Media_Installed mediaInstalled = selectedmedia == 0
		? MediaInfo.SDCardInfo : MediaInfo.GameCartInfo;

    /* Grey Outlines */
    if (selectedgame == 1) //Orig ACNL
        C2D_DrawRectSolid(98, 63, 0, 54, 54, COLOR_GREY);

    else if (selectedgame == 2) //WA ACNL
        C2D_DrawRectSolid(166, 63, 0, 54, 54, COLOR_GREY);

    if (selectedregion == 0) //JPN
        C2D_DrawRectSolid(167, 127, 0, 50, 36, COLOR_GREY);

    else if (selectedregion == 1) //USA
        C2D_DrawRectSolid(103, 127, 0, 50, 33, COLOR_GREY);

    else if (selectedregion == 2) //EUR
        C2D_DrawRectSolid(39, 127, 0, 50, 36, COLOR_GREY);

    else if (selectedregion == 3) //KOR
        C2D_DrawRectSolid(231, 127, 0, 50, 36, COLOR_GREY);

    DrawSprite(GameSelect_ss, ACNL_ICON, 101, 66);
    DrawSprite(GameSelect_ss, ACNL_WA_ICON, 169, 66);
    DrawSprite(GameSelect_ss, EUR_FLAG, 42, 130);
    DrawSprite(GameSelect_ss, USA_FLAG, 106, 127);
    DrawSprite(GameSelect_ss, JPN_FLAG, 170, 130);
    DrawSprite(GameSelect_ss, KOR_FLAG, 234, 130);
    DrawText(128, 195, 0.7, 0.7, COLOR_GREY, "Confirm"); //Confirm Button Text

    if (selectedmedia == -1 || !mediaInstalled.InstalledTitles.ORIG_installed) //Grey out Orig ACNL Icon if no ORIG ACNL game is found
    C2D_DrawRectSolid(101, 66, 0, 48, 48, COLOR_GREY_FILTER);

    if (selectedmedia == -1 || !mediaInstalled.InstalledTitles.WA_installed) //Grey out WA ACNL Icon if no WA ACNL game is found
    C2D_DrawRectSolid(169, 66, 0, 48, 48, COLOR_GREY_FILTER);

    if (selectedgame == 1)
    {
        if (!mediaInstalled.InstalledTitles.ORIG_JPN_installed) //Grey out JPN Flag if no orig JPN game is found
            C2D_DrawRectSolid(170, 130, 0, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.ORIG_USA_installed) //Grey out USA Flag if no orig USA game is found
            C2D_DrawRectSolid(106, 130, 0, 44, 27, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.ORIG_EUR_installed) //Grey out EUR Flag if no orig EUR game is found
            C2D_DrawRectSolid(42, 130, 0, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.ORIG_KOR_installed) //Grey out KOR Flag if no orig KOR game is found
            C2D_DrawRectSolid(234, 130, 0, 44, 30, COLOR_GREY_FILTER);
    }

    else if (selectedgame == 2)
    {
        if (!mediaInstalled.InstalledTitles.WA_JPN_installed) //Grey out JPN Flag if no WA JPN game is found
            C2D_DrawRectSolid(170, 130, 0, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.WA_USA_installed) //Grey out USA Flag if no WA USA game is found
            C2D_DrawRectSolid(106, 130, 0, 44, 27, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.WA_EUR_installed) //Grey out EUR Flag if no WA EUR game is found
            C2D_DrawRectSolid(42, 130, 0, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.WA_KOR_installed) //Grey out KOR Flag if no WA KOR game is found
            C2D_DrawRectSolid(234, 130, 0, 44, 30, COLOR_GREY_FILTER);
    }

    else
    {
        C2D_DrawRectSolid(170, 130, 0, 44, 30, COLOR_GREY_FILTER); //JPN
        C2D_DrawRectSolid(106, 130, 0, 44, 27, COLOR_GREY_FILTER); //USA
        C2D_DrawRectSolid(42, 130, 0, 44, 30, COLOR_GREY_FILTER); //EUR
        C2D_DrawRectSolid(234, 130, 0, 44, 30, COLOR_GREY_FILTER); //KOR
    }

    C2D_SceneBegin(top);
    draw_centered_text(0, 400, 90, 0, 0.8, 0.8, COLOR_BLACK, "Select your installed ACNL game:");
    draw_cursor();
    C3D_FrameEnd(0);
}