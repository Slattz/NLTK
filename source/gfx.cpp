#include "gfx.h"
#include "about.h"

void InitCommonGFX(void)
{
    //About Menu
    pp2d_load_texture_png(TWITTER_ICON,        "romfs:/Icons/About/Twitter.png");
    pp2d_load_texture_png(DISCORD_ICON,        "romfs:/Icons/About/Discord.png");   
    //Common
    pp2d_load_texture_png(NLTK_ICON,           "romfs:/Menus/Common/icon.png");
    pp2d_load_texture_png(CHECKBOX_EMPTY,      "romfs:/Menus/Common/checkbox.png");
    pp2d_load_texture_png(CHECKBOX_FILLED,     "romfs:/Menus/Common/checkbox_c.png");
    pp2d_load_texture_png(CURSOR_POINT,        "romfs:/Menus/Common/Hand/hand_point.png");
    pp2d_load_texture_png(CURSOR_SELECT,       "romfs:/Menus/Common/Hand/hand_select.png");
    pp2d_load_texture_png(ITEM_COMMON,         "romfs:/Icons/Common/itm_default.png");
}

void ExitCommonGFX(void)
{
	//About Menu
	pp2d_free_texture(TWITTER_ICON);
	pp2d_free_texture(DISCORD_ICON);
	//Common 
	pp2d_free_texture(NLTK_ICON);
	pp2d_free_texture(CHECKBOX_EMPTY);
	pp2d_free_texture(CHECKBOX_FILLED);
	pp2d_free_texture(CURSOR_POINT);
	pp2d_free_texture(CURSOR_SELECT);
	pp2d_free_texture(ITEM_COMMON);
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

    pp2d_set_screen_color(GFX_TOP, COLOR_GREENBACKGROUND);
    pp2d_set_screen_color(GFX_BOTTOM, COLOR_GREENBACKGROUND);
    pp2d_begin_draw(GFX_TOP, GFX_LEFT);
    pp2d_draw_rectangle(0, 0, 400, 20, COLOR_MENU_BARS);
    pp2d_draw_text(titleX, 2, 0.5, 0.5, COLOR_GREY, title);
    if (config.isdebug)
        pp2d_draw_text(3, 2, 0.5, 0.5, COLOR_ORANGE, "Debug: ON");
}

void draw_cursor(void)
{
    pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
    updatePointerPosition();

    if (g_cursorpos.show)
    {
        if (g_cursorpos.A_held)
            pp2d_draw_texture(CURSOR_SELECT, g_cursorpos.x, g_cursorpos.y);
        
        else
            pp2d_draw_texture(CURSOR_POINT, g_cursorpos.x, g_cursorpos.y);
    } 
}

void infoDisp(gfxScreen_t target, const char* message)
{
    char     temp[1024] = {0};
    strncpy(temp, message, 1023);
    strncat(temp, "\n\nPress " FONT_A " to Continue.", 40);

    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_A) break;
        
        draw_base_interface();
        pp2d_draw_on(target, GFX_LEFT);
        pp2d_draw_rectangle(50, 33.5, 300, 180, COLOR_BROWN);
        pp2d_draw_text_center(target, 80, 0.7, 0.7, COLOR_GREY, temp);
        pp2d_end_draw();
    }
}

void infoDispF(gfxScreen_t target, const char* string, ...)
{
    char       buffer[1024];
    va_list    args;
    va_start(args, string);
    vsnprintf(buffer, sizeof(buffer), string, args);
    infoDisp(target, buffer);
    va_end(args);
}

bool confirmDisp(gfxScreen_t target, const char* message)
{
    char     temp[1024] = {0};
    strncpy(temp, message, 1023);
    strncat(temp, "\n\nYes: " FONT_A ", No: " FONT_B, 40);

    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_A) return 1;
        if (hidKeysDown() & KEY_B) return 0;
        
        draw_base_interface();
        pp2d_draw_on(target, GFX_LEFT);
        pp2d_draw_rectangle(50, 33.5, 300, 180, COLOR_BROWN);
        pp2d_draw_text_center(target, 80, 0.7, 0.7, COLOR_GREY, temp);
        pp2d_end_draw();
    }
    return 0;
}

void confirmDispF(gfxScreen_t target, const char* string, ...)
{
    char       buffer[1024];
    va_list    args;
    va_start(args, string);
    vsnprintf(buffer, sizeof(buffer), string, args);
    confirmDisp(target, buffer);
    va_end(args);
}

void DisplayText(gfxScreen_t target, int x, int y, int width, int height, float textsize, const char* message)
{
	char     temp[1024] = { 0 };
	strncpy(temp, message, 1023);
	pp2d_draw_on(target, GFX_LEFT);
	pp2d_draw_rectangle(x, y, width, height, COLOR_BROWN);
	pp2d_draw_text_wrap(x + 5, y + 5, textsize, textsize, COLOR_GREY, width - 5, temp);
	//Caller function must call pp2d_end_draw();
}

void DisplayTextF(gfxScreen_t target, int x, int y, int width, int height, float textsize, const char* string, ...)
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
		pp2d_draw_on(GFX_TOP, GFX_LEFT);
		pp2d_draw_rectangle(50, 33.5, 300, 180, COLOR_BROWN);
		pp2d_draw_text_center(GFX_TOP, 100, 0.5, 0.5, COLOR_GREY, cardRemovedErrorMessage);

		if (show || IsGameCartInserted()) {
			pp2d_draw_texture(GAME_CART, 176, 40); // NOTE: Top Screen resolution 400x240, Bottom Screen Resolution: 320x240
		}
		else {
			pp2d_draw_texture_blend(GAME_CART, 176, 40, COLOR_GREY_FILTER);
		}

		pp2d_end_draw();

		timer++;
		if (timer >= maxTimer) {
			timer = 0;
			show = !show;
		}
	}
}

void draw_config_menu(void)
{
    const char* ConfigText[] = {"Auto Update", "Auto Save Backup", "Auto Load Preferred Game", "Debug"};
    bool  ConfigBools[] = {config.autoupdate, config.autosavebackup, config.autoloadprefGame, 
                    config.isdebug};
    u32 configmax = sizeof(ConfigBools)/sizeof(ConfigBools[0]);

    draw_base_interface();
    pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);

    u32 i;
    for (i = 0; i < configmax-1; i++) //configmax-1: exclude Debug
    {
        if (ConfigBools[i])
            pp2d_draw_texture(CHECKBOX_FILLED, 20, 20+(28*i));

        else
            pp2d_draw_texture(CHECKBOX_EMPTY, 20, 20+(28*i));

        pp2d_draw_text(52, 21+(28*i), 0.6, 0.6, COLOR_GREY, ConfigText[i]); //ConfigText
    }

    if (config.isdebug)
    {
        pp2d_draw_texture(CHECKBOX_FILLED, 20, 20+(28*(configmax-1)));
        pp2d_draw_text(52, 21+(28*(configmax-1)), 0.6, 0.6, COLOR_GREY, ConfigText[configmax-1]);
    }
    draw_cursor();
    pp2d_end_draw();
}

void draw_about_menu(bool discord, bool twitter)
{
    draw_base_interface();
    pp2d_draw_text(175, 30, 0.6, 0.6, COLOR_GREY, "Credits:");
    for (int i = 0; i < creditsCount; i++)
    {
        pp2d_draw_text(10, 55+(i*20), 0.46, 0.46, COLOR_GREY, credits[i*2]); //Name
        pp2d_draw_text(120, 55+(i*20), 0.45, 0.45, COLOR_GREY, credits[1+i*2]); //Description
    }
    pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
    pp2d_draw_texture(NLTK_ICON, 126, 10); //NLTK's Icon
    pp2d_draw_texture(DISCORD_ICON, 55, 180); //Discord Icon
    pp2d_draw_texture(TWITTER_ICON, 215, 180); //Twitter Icon
    /* L:55, M:135, R:215 */

    if (discord)
        DisplayText(GFX_BOTTOM, 60, 100, 200, 70, 0.5, discordtext);

    if (twitter)
        DisplayText(GFX_BOTTOM, 60, 100, 200, 70, 0.5, twittertext);

    draw_cursor();
    pp2d_end_draw();
}

void draw_game_select_menu(int selectedgame, int selectedregion, int selectedmedia)
{
    draw_base_interface();
    pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);

	if (selectedmedia == -1 || selectedgame == -1 || selectedregion == -1) {
		pp2d_draw_rectangle(75, 190, 170, 30, COLOR_BROWN); //Confirm Button
	}
	else {
		pp2d_draw_rectangle(75, 190, 170, 30, COLOR_LIGHT_BROWN); //Confirm Button (Can Confirm)
	}

	// Media Type
	if (selectedmedia == 1) { //Game Card
		pp2d_draw_rectangle(98, 3, 54, 54, COLOR_GREY); //Grey Outline
		pp2d_draw_texture(GAME_CART, 101, 6); //Game Card is selected
		pp2d_draw_texture_blend(SD_CARD, 169, 6, COLOR_GREY_FILTER);
	}
	else if (selectedmedia == 0) { //SD Card
        pp2d_draw_rectangle(166, 3, 54, 54, COLOR_GREY); //Grey Outline
        pp2d_draw_texture(SD_CARD, 169, 6); //SD Card is selected
		pp2d_draw_texture_blend(GAME_CART, 101, 6, COLOR_GREY_FILTER);
	}
	else { //No Media selected yet
		if (MediaInfo.GameCartInfo.HasACNLData) {
			pp2d_draw_texture(GAME_CART, 101, 6);
		}
		else {
			pp2d_draw_texture_blend(GAME_CART, 101, 6, COLOR_GREY_FILTER);
		}

		if (MediaInfo.SDCardInfo.HasACNLData) {
			pp2d_draw_texture(SD_CARD, 169, 6);
		}
		else {
			pp2d_draw_texture_blend(SD_CARD, 169, 6, COLOR_GREY_FILTER);
		}
	}

	NLTK_Media_Installed mediaInstalled = selectedmedia == 0
		? MediaInfo.SDCardInfo : MediaInfo.GameCartInfo;

    /* Grey Outlines */
    if (selectedgame == 1) //Orig ACNL
        pp2d_draw_rectangle(98, 63, 54, 54, COLOR_GREY);

    else if (selectedgame == 2) //WA ACNL
        pp2d_draw_rectangle(166, 63, 54, 54, COLOR_GREY);

    if (selectedregion == 0) //JPN
        pp2d_draw_rectangle(167, 130, 50, 36, COLOR_GREY);

    else if (selectedregion == 1) //USA
        pp2d_draw_rectangle(103, 130, 50, 33, COLOR_GREY);

    else if (selectedregion == 2) //EUR
        pp2d_draw_rectangle(39, 130, 50, 36, COLOR_GREY);

    else if (selectedregion == 3) //KOR
        pp2d_draw_rectangle(231, 130, 50, 36, COLOR_GREY);

    pp2d_draw_texture(ACNL_ICON, 101, 66);
    pp2d_draw_texture(ACNL_WA_ICON, 169, 66);
    pp2d_draw_texture(EUR_FLAG, 42, 130);
    pp2d_draw_texture(USA_FLAG, 106, 127);
    pp2d_draw_texture(JPN_FLAG, 170, 130);
    pp2d_draw_texture(KOR_FLAG, 234, 130);
    pp2d_draw_text(128, 195, 0.7, 0.7, COLOR_GREY, "Confirm"); //Confirm Button Text

    if (selectedmedia == -1 || !mediaInstalled.InstalledTitles.ORIG_installed) //Grey out Orig ACNL Icon if no ORIG ACNL game is found
    pp2d_draw_rectangle(101, 66, 48, 48, COLOR_GREY_FILTER);

    if (selectedmedia == -1 || !mediaInstalled.InstalledTitles.WA_installed) //Grey out WA ACNL Icon if no WA ACNL game is found
    pp2d_draw_rectangle(169, 66, 48, 48, COLOR_GREY_FILTER);

    if (selectedgame == 1)
    {
        if (!mediaInstalled.InstalledTitles.ORIG_JPN_installed) //Grey out JPN Flag if no orig JPN game is found
            pp2d_draw_rectangle(170, 130, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.ORIG_USA_installed) //Grey out USA Flag if no orig USA game is found
            pp2d_draw_rectangle(106, 130, 44, 27, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.ORIG_EUR_installed) //Grey out EUR Flag if no orig EUR game is found
            pp2d_draw_rectangle(42, 130, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.ORIG_KOR_installed) //Grey out KOR Flag if no orig KOR game is found
            pp2d_draw_rectangle(234, 130, 44, 30, COLOR_GREY_FILTER);
    }

    else if (selectedgame == 2)
    {
        if (!mediaInstalled.InstalledTitles.WA_JPN_installed) //Grey out JPN Flag if no WA JPN game is found
            pp2d_draw_rectangle(170, 130, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.WA_USA_installed) //Grey out USA Flag if no WA USA game is found
            pp2d_draw_rectangle(106, 130, 44, 27, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.WA_EUR_installed) //Grey out EUR Flag if no WA EUR game is found
            pp2d_draw_rectangle(42, 130, 44, 30, COLOR_GREY_FILTER);
    
        if (!mediaInstalled.InstalledTitles.WA_KOR_installed) //Grey out KOR Flag if no WA KOR game is found
            pp2d_draw_rectangle(234, 130, 44, 30, COLOR_GREY_FILTER);
    }

    else
    {
        pp2d_draw_rectangle(170, 130, 44, 30, COLOR_GREY_FILTER); //JPN
        pp2d_draw_rectangle(106, 130, 44, 27, COLOR_GREY_FILTER); //USA
        pp2d_draw_rectangle(42, 130, 44, 30, COLOR_GREY_FILTER); //EUR
        pp2d_draw_rectangle(234, 130, 44, 30, COLOR_GREY_FILTER); //KOR
    }

    pp2d_draw_on(GFX_TOP, GFX_LEFT);
    pp2d_draw_text_center(GFX_TOP, 90, 0.8, 0.8, COLOR_BLACK, "Select your installed ACNL game:");
    draw_cursor();
    pp2d_end_draw();
}