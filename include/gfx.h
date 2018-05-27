#pragma once

#ifndef GFX_H
#define GFX_H

#include "common.h"
#include "SpriteIDs.h"

typedef struct
{
    C2D_Sprite spr;
    float dx, dy; // velocity
} Sprite;

extern C3D_RenderTarget* top;
extern C3D_RenderTarget* bottom;
extern C2D_ImageTint* GreyFilter;
extern C2D_ImageTint* GreenFilter;

extern C2D_SpriteSheet About_ss;
extern C2D_SpriteSheet Acres_ss;
extern C2D_SpriteSheet Common_ss;
extern C2D_SpriteSheet GameSelect_ss;
extern C2D_SpriteSheet Editor_ss;
extern C2D_SpriteSheet Items_ss;
extern C2D_SpriteSheet Players_ss;

void InitGFX(void);
void ExitGFX(void);
void DrawSprite(C2D_SpriteSheet sheet, size_t imgindex, float x, float y, const C2D_ImageTint *tint = nullptr, float scaleX = 1.0, float scaleY = 1.0);
void DrawText(float x, float y, float scaleX, float scaleY, u32 color, const char* text, bool atBaseline = false);
void infoDisp(C3D_RenderTarget *target, const char* message);
bool confirmDisp(C3D_RenderTarget *target, const char* message);
void DisplayText(C3D_RenderTarget *target, int x, int y, int width, int height, float textsize, const char* message);
void infoDispF(C3D_RenderTarget *target, const char* string, ...);
void confirmDispF(C3D_RenderTarget *target, const char* string, ...);
void DisplayTextF(C3D_RenderTarget *target, int x, int y, int width, int height, float textsize, const char* string, ...);
void DisplayCardError();

void draw_base_interface(void);
void draw_cursor(void);
void draw_main_menu(void);
void draw_game_select_menu(int selectedgame, int selectedregion, int selectedmedia);
void draw_about_menu(bool discord, bool twitter);
void draw_config_menu(void);

//Colors
static const u32 COLOR_BG              =    C2D_Color32(92,188,105,255);
static const u32 COLOR_MENU_BARS       =    C2D_Color32(33,139,43,255);
static const u32 COLOR_WHITE           =    C2D_Color32(255,255,255,255);
static const u32 COLOR_BLACK           =    C2D_Color32(0,0,0,255);
static const u32 COLOR_GREEN           =    C2D_Color32(124,255,142,255);
static const u32 COLOR_BROWN           =    C2D_Color32(80,78,45,255);
static const u32 COLOR_LIGHT_BROWN     =    C2D_Color32(99,93,31,255);
static const u32 COLOR_GREY_FILTER     =    C2D_Color32(0,0,0,170);
static const u32 COLOR_GREY            =    C2D_Color32(210,210,210,255);
static const u32 COLOR_RED             =    C2D_Color32(255,0,0,255);
static const u32 COLOR_YELLOW          =    C2D_Color32(255,255,0,255);
static const u32 COLOR_ORANGE          =    C2D_Color32(255,106,0,255);
static const u32 COLOR_TRANSPARENT     =    C2D_Color32(0,0,0,0);

/* Image IDs */;

//Main Menu
#define TOWN_ICON           Editor_Town_idx
#define ACRES_ICON          Editor_Acres_idx
#define PROFILE_ICON        Editor_Profile_idx
#define VILLAGERS_ICON      Editor_Villagers_idx
#define PWP_ICON            Editor_Pwp_idx
#define ISLAND_ICON         Editor_Island_idx
#define MAIN_STREET_ICON    Editor_Main_Street_idx
#define ABOUT_ICON          Editor_About_idx
#define BUTTON_MAIN         Editor_Button_idx
//Game Select Menu
#define EUR_FLAG            GameSelect_Europe_idx
#define USA_FLAG            GameSelect_USA_idx
#define JPN_FLAG            GameSelect_Japan_idx
#define KOR_FLAG            GameSelect_Korea_idx
#define ACNL_ICON           GameSelect_Icon_idx
#define ACNL_WA_ICON        GameSelect_IconWA_idx
#define GAME_CART			GameSelect_GameCart_idx
#define SD_CARD				GameSelect_SDCard_idx
//Town Menu
//Players Menu
#define NO_TPC_PIC          Players_NoTPC_idx
#define PLYR_ABOUT          Players_About_idx
#define PLYR_INVENTORY      Players_Inventory_idx
#define PLYR_APPEARANCE     Players_Appearance_idx
#define PLYR_HOUSE          Players_House_idx
#define PLYR_PATTERN        Players_Patterns_idx
#define PLYR_MAILBOX        Players_Mailbox_idx
#define PLYR_MAILBOX_UNREAD Players_Mailbox_Unread_idx

//Pwps Menu
//Main Street Menu
//Acres Menu
//Villagers Menu
//Island Menu

//About Menu
#define TWITTER_ICON        About_Twitter_idx
#define DISCORD_ICON        About_Discord_idx

//Others
#define NLTK_ICON           Common_icon_idx
#define CHECKBOX_EMPTY      Common_checkbox_idx
#define CHECKBOX_FILLED     Common_checkbox_c_idx
#define CURSOR_POINT        Common_hand_point_idx
#define CURSOR_SELECT       Common_hand_select_idx
#define EDITOR_ICON         Common_Editor_idx
#define MANAGER_ICON        Common_Manager_idx

/* Item Icons */
#define ITEM_HOLE           Common_ItemHole_idx

/* Reserve each acre pic*/
#define ACRE_PNG            Acres_acre_0_idx
#define LAST_ACRE_PIC       Acres_acre_214_idx


#endif