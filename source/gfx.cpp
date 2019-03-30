#include <3ds.h>
#include "CTRFont.hpp"
#include "InputManager.h"
#include "unicode.h"
#include "common.h"
#include "structs.h"
#include "config.hpp"
#include "gfx.h"
#include "utils.h"
#include "nfs.h"

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
C2D_SpriteSheet Villagers_ss;
C2D_SpriteSheet Villagers2_ss;

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

struct C2D_SpriteSheet_s
{
    Tex3DS_Texture t3x;
    C3D_Tex        tex;
};

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
    Villagers_ss = C2D_SpriteSheetLoad("romfs:/gfx/Villagers_ss.t3x");
    Villagers2_ss = C2D_SpriteSheetLoad("romfs:/gfx/Villagers2_ss.t3x");
    C3D_TexSetFilter(&About_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Acres_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Common_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&GameSelect_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Editor_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Items_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Players_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Swkbd_ss->tex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&Villagers_ss->tex, GPU_LINEAR, GPU_LINEAR);

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
    static const u32 titleX = 320;

    if (!Init) {

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

    if (Config::Instance()->IsDebug)
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
        InputManager::Instance()->RefreshInput();
        if (InputManager::Instance()->IsButtonDown(KEY_A) && Type != MsgTypeNone) return true;
        if (InputManager::Instance()->IsButtonDown(KEY_B) && Type == MsgTypeConfirm) return false;
        
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
    if (FS::IsGameCartInserted() && checkGameCartIsACNL()) {
        return;
    }

    bool show = true;
    u32 timer = 0;
    u32 maxTimer = 60; // 60 FPS?
    const char *cardRemovedErrorMessage = "To continue editing, please reinsert your\nAnimal Crossing: New Leaf Game Card.\n" \
        "Press " FONT_A " when done.";

    Text cardRemovedErrorText(Color(COLOR_GREY), cardRemovedErrorMessage, 0.75f, 0.75f);
    cardRemovedErrorText.CenterInBounds(55.f, 42.5f, 295.f, 175.f);

    while (aptMainLoop())
    {
        InputManager::Instance()->RefreshInput();
        if (FS::IsGameCartInserted() && checkGameCartIsACNL() && InputManager::Instance()->IsButtonDown(KEY_A)) break;

        draw_base_interface();
        C2D_SceneBegin(top);
        C2D_DrawRectSolid(50, 33.5, 0, 300, 180, COLOR_BROWN);
        cardRemovedErrorText.Draw();

        if (show || FS::IsGameCartInserted()) {
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
    if (Config::Instance()->IsDebug) DrawSprite(Common_ss, MANAGER_ICON, 180, 90); //Manager Icon
    ModeText[0].Draw(); //Editor
    if (Config::Instance()->IsDebug) ModeText[1].Draw(); //Manager
    
    C2D_SceneBegin(top);
    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}