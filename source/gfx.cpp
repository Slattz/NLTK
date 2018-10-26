#include <3ds.h>
#include "CTRFont.hpp"
#include "unicode.h"
#include "common.h"
#include "structs.h"
#include "about.h"
#include "cursor.h"
#include "config.hpp"
#include "gfx.h"
#include "utils.h"

extern Config *config;
extern NLTK_Titles_Info MediaInfo;

C3D_RenderTarget* top;
C3D_RenderTarget* bottom;
C2D_ImageTint* GreyFilter = new C2D_ImageTint[sizeof(C2D_ImageTint)];
C2D_ImageTint* GreySelectFilter = new C2D_ImageTint[sizeof(C2D_ImageTint)];
C2D_ImageTint* GreenFilter = new C2D_ImageTint[sizeof(C2D_ImageTint)];

C2D_SpriteSheet About_ss;
C2D_SpriteSheet Acres_ss;
C2D_SpriteSheet Common_ss;
C2D_SpriteSheet GameSelect_ss;
C2D_SpriteSheet Editor_ss;
C2D_SpriteSheet Items_ss;
C2D_SpriteSheet Players_ss;
C2D_SpriteSheet Swkbd_ss;

const u32 COLOR_BG              =    C2D_Color32(92,188,105,255);
const u32 COLOR_MENU_BARS       =    C2D_Color32(33,139,43,255);
const u32 COLOR_WHITE           =    C2D_Color32(255,255,255,255);
const u32 COLOR_BLACK           =    C2D_Color32(0,0,0,255);
const u32 COLOR_GREEN           =    C2D_Color32(124,255,142,255);
const u32 COLOR_BROWN           =    C2D_Color32(80,78,45,255);
const u32 COLOR_LIGHT_BROWN     =    C2D_Color32(99,93,31,255);
const u32 COLOR_GREY_FILTER     =    C2D_Color32(0,0,0,170);
const u32 COLOR_GREY            =    C2D_Color32(210,210,210,255);
const u32 COLOR_DARK_GREY       =    C2D_Color32(145,145,145,255);
const u32 COLOR_RED             =    C2D_Color32(255,0,0,255);
const u32 COLOR_YELLOW          =    C2D_Color32(255,255,0,255);
const u32 COLOR_ORANGE          =    C2D_Color32(255,106,0,255);
const u32 COLOR_TRANSPARENT     =    C2D_Color32(0,0,0,0);

Cursor g_cursor;

    struct C2D_SpriteSheet_s
{
    Tex3DS_Texture t3x;
    C3D_Tex        tex;
};

//std::vector<Text> DebugText;
/*
    if (!TextInit) {
        ButtonText.push_back(Text(COLOR_GREY, "Town", TextSize, TextSize, 37.f, 147.f));
    }

    if (config->IsDebug)
    {
        Text Cursor()
        DrawText(100, 120, 0.5, 0.5, COLOR_GREY, Format("Cursor X: %d, Cursor Y: %d", g_cursorpos.x, g_cursorpos.y).c_str());
        DrawText(100, 140, 0.5, 0.5, COLOR_GREY, Format("Game + Region: 0x%016llX", g_tid).c_str());
        DrawText(100, 160, 0.5, 0.5, COLOR_GREY, Format("Is ACNL: %d", is_ACNL(g_tid)).c_str());
        DrawText(100, 180, 0.5, 0.5, COLOR_GREY, Format("ItemBin allocated: %d", (g_ItemBin==NULL) ? 0:1).c_str());
    }*/

void InitGFX(void)
{
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    /* Load Spritesheets */
    About_ss = C2D_SpriteSheetLoad("romfs:/gfx/About_ss.t3x");
    Acres_ss = C2D_SpriteSheetLoad("romfs:/gfx/Acres_ss.t3x");
    Common_ss = C2D_SpriteSheetLoad("romfs:/gfx/Common_ss.t3x");
    GameSelect_ss = C2D_SpriteSheetLoad("romfs:/gfx/GameSelect_ss.t3x");
    Editor_ss = C2D_SpriteSheetLoad("romfs:/gfx/Editor_ss.t3x");
    Items_ss = C2D_SpriteSheetLoad("romfs:/gfx/Items_ss.t3x");
    Players_ss = C2D_SpriteSheetLoad("romfs:/gfx/Players_ss.t3x");
    Swkbd_ss = C2D_SpriteSheetLoad("romfs:/gfx/Swkbd_ss.t3x");
    C3D_TexSetFilter(&About_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Acres_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Common_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&GameSelect_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Editor_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Items_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Players_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Swkbd_ss->tex, GPU_LINEAR, GPU_LINEAR);

    /* Very Hacky, used as a bypass for a tex3ds bug */
    C2D_Image button = C2D_SpriteSheetGetImage(Editor_ss, BUTTON_MAIN);
    C2D_Image swkbd_shifton = C2D_SpriteSheetGetImage(Swkbd_ss, SWKBD_SHIFT_ON);
    C2D_Image swkbd_shiftoff = C2D_SpriteSheetGetImage(Swkbd_ss, SWKBD_SHIFT_OFF);
    C2D_Image swkbd_confirm = C2D_SpriteSheetGetImage(Swkbd_ss, SWKBD_CONFIRM);
    ((Tex3DS_SubTexture *)button.subtex)->right -= 0.005f;
    ((Tex3DS_SubTexture *)swkbd_shifton.subtex)->right -= 0.010f;
    ((Tex3DS_SubTexture *)swkbd_shiftoff.subtex)->right -= 0.010f;
    ((Tex3DS_SubTexture *)swkbd_confirm.subtex)->right -= 0.010f;

    /* Init Tints */
    C2D_PlainImageTint(GreyFilter, COLOR_GREY_FILTER, 1.0f);
    C2D_PlainImageTint(GreySelectFilter, COLOR_GREY_FILTER, 0.8f);
    C2D_PlainImageTint(GreenFilter, COLOR_GREEN, 1.0f);
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

void DrawSprite(C2D_SpriteSheet sheet, size_t imgindex, float x, float y, const C2D_ImageTint *tint, float scaleX, float scaleY, float z)
{
    C2D_Image img = C2D_SpriteSheetGetImage(sheet, imgindex);
    C2D_DrawImageAt(img, x, y, z, tint, scaleX, scaleY);
}

void draw_base_interface(void)
{
    static bool Init = false;
    static std::vector<Text> BarText;
    static std::string title = TITLE " " VERSION;
    static int titleX = 320;

    if (!Init) {
        if (BETA_VERSION > 0) {
            title += " B" TOSTRING(BETA_VERSION);
            titleX -= 20;
        }

        BarText.push_back(Text(COLOR_GREY, title, 0.6, 0.6, titleX, 2.f));
        BarText.push_back(Text(COLOR_ORANGE, "Debug: ON", 0.6, 0.6, 3.f, 2.f));
        Init = true;
    }

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, COLOR_BG);
    C2D_TargetClear(bottom, COLOR_BG);

    C2D_SceneBegin(top);
    C2D_DrawRectSolid(0, 0, 0, 400, 20, COLOR_MENU_BARS);
    BarText[0].Draw();

    if (config->IsDebug)
        BarText[1].Draw();
    C2D_Flush();
}

bool MsgDisp(C3D_RenderTarget *target, std::string message, MsgType Type, u32 bgColor, u32 textColor, 
    float textsize, float x, float y, float width, float height)
{
    static const std::string acceptMsg = "\n\nPress " FONT_A " to Continue.";
    static const std::string confirmMsg = "\n\nYes: " FONT_A ", No: " FONT_B;

    if (Type == MsgTypeAccept) {
        message += acceptMsg;
    }

    else if (Type == MsgTypeConfirm) {
        message += confirmMsg;
    }

    Text Msg(textColor, message, textsize, textsize);
    Msg.CenterInBounds(x+5, y+5, width-5, height-5);


    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_A && Type != MsgTypeNone) return true;
        if (hidKeysDown() & KEY_B && Type == MsgTypeConfirm) return false;
        
        if (Type != MsgTypeNone)
            draw_base_interface();

        C2D_SceneBegin(target);
        C2D_DrawRectSolid(x, y, 0, width, height, bgColor);
        Msg.Draw();

        if (Type == MsgTypeNone) break;
        C3D_FrameEnd(0);
    }
    return false;
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
    static bool TextInit = false;
    static const float TextSize = 0.7f;
    static std::vector<Text> ModeText;
    static std::vector<Text> ColumnText;

    if (!TextInit) {
        ModeText.push_back(Text(COLOR_GREY, "Editor", TextSize, TextSize, 100.f, 150.f));
        ModeText.push_back(Text(COLOR_GREY, "Manager", TextSize, TextSize, 180.f, 150.f));
        
        ColumnText.push_back(Text(COLOR_GREY, "About", TextSize, TextSize, 42.f, 38.f));
        ColumnText.push_back(Text(COLOR_GREY, "Options", TextSize, TextSize, 237.f, 38.f));
        TextInit = true;
    }

    draw_base_interface();
    C2D_SceneBegin(bottom);
    DrawSprite(Common_ss, NLTK_ICON, 126.f, 10.f); //NLTK's Icon
    DrawSprite(Editor_ss, BUTTON_MAIN, 20.f,  30.f, nullptr, 1.15f, 0.6f);  //w = 80, h = 33
    DrawSprite(Editor_ss, BUTTON_MAIN, 220.f, 30.f, nullptr, 1.15f, 0.6f);  //w = 80, h = 33
    ColumnText[0].Draw(); //About
    ColumnText[1].Draw(); //Options

    DrawSprite(Common_ss, EDITOR_ICON, 90, 90); //Editor Icon
    DrawSprite(Common_ss, MANAGER_ICON, 180, 90); //Manager Icon
    ModeText[0].Draw(); //Editor
    ModeText[1].Draw(); //Manager
    
    C2D_SceneBegin(top);
    //draw_centered_text(0, 400, 80, 0, 1.1, 1.1, COLOR_GREY, "Actual Main Menu!");
    g_cursor.Draw();
    C3D_FrameEnd(0);
}

void draw_config_menu(void)
{
    static bool TextInit = false;
    static const float TextSize = 0.7f;
    static std::vector<Text> ConfigText;
    bool  ConfigBools[] = {config->Auto_Update, config->Auto_SaveBackup, config->Auto_loadprefGame, 
                    config->IsDebug};
    u32 configamount = 4;

    if (!TextInit) {
        ConfigText.push_back(Text(COLOR_GREY, "Auto Update", TextSize, TextSize));
        ConfigText.push_back(Text(COLOR_GREY, "Auto Save Backup", TextSize, TextSize));
        ConfigText.push_back(Text(COLOR_GREY, "Auto Load Preferred Game", TextSize, TextSize));
        ConfigText.push_back(Text(COLOR_GREY, "Debug", TextSize, TextSize));
        TextInit = true;
    }

    draw_base_interface();
    C2D_SceneBegin(bottom);

    u32 i;
    for (i = 0; i < configamount-1; i++) //configmax-1: exclude Debug
    {
        DrawSprite(Common_ss, (ConfigBools[i] == true ? CHECKBOX_FILLED : CHECKBOX_EMPTY), 20, 20+(28*i));
        ConfigText[i].Draw(52, 23+(28*i)); //ConfigText
    }

    if (config->IsDebug)
    {
        DrawSprite(Common_ss, CHECKBOX_FILLED, 20, 20+(28*(configamount-1)));
        ConfigText[configamount-1].Draw(52, 23+(28*(configamount-1)));
    }
    g_cursor.Draw();
    C3D_FrameEnd(0);
}

void draw_about_menu(bool discord, bool twitter)
{
    static bool TextInit = false;
    static const float TextSize = 0.6f;
    static std::vector<Text> AboutLText;
    static std::vector<Text> AboutRText;
    static Text CreditTxt(COLOR_GREY, "Credits:", TextSize, TextSize, 175, 30);

    if (!TextInit) {
        for (int i = 0; i < creditsCount; i++) {
            AboutLText.push_back(Text(COLOR_GREY, credits[i*2], TextSize, TextSize));
            AboutRText.push_back(Text(COLOR_GREY, credits[1+i*2], TextSize, TextSize));
        }
        TextInit = true;
    }

    draw_base_interface();
    CreditTxt.Draw();

    for (int i = 0; i < creditsCount; i++)
    {
        AboutLText[i].Draw(10, 55+(i*20)); //Name
        AboutRText[i].Draw(120, 55+(i*20)); //Description
    }

    C2D_SceneBegin(bottom);
    DrawSprite(Common_ss, NLTK_ICON, 126, 10); //NLTK's Icon
    DrawSprite(About_ss, DISCORD_ICON, 55, 180); //Discord Icon
    DrawSprite(About_ss, TWITTER_ICON, 215, 180); //Twitter Icon
    /* L:55, M:135, R:215 */

    if (discord)
        MsgDisp(bottom, discordtext, MsgTypeNone, COLOR_BROWN, COLOR_GREY, 0.6f, 60.f, 100.f, 200.f, 70.f);

    else if (twitter)
        MsgDisp(bottom, twittertext, MsgTypeNone, COLOR_BROWN, COLOR_GREY, 0.6f, 60.f, 100.f, 200.f, 70.f);

    g_cursor.Draw();
    C3D_FrameEnd(0);
}

void draw_game_select_menu(int selectedgame, int selectedregion, int selectedmedia)
{
    static const Text ConfirmTxt(COLOR_GREY, "Confirm", 0.7f, 0.7f, 128, 195);
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
    ConfirmTxt.Draw(); //Confirm Button Text

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
    g_cursor.Draw();
    C3D_FrameEnd(0);
}