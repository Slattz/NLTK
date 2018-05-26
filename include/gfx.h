#pragma once

#ifndef GFX_H
#define GFX_H

#include "common.h"

typedef struct
{
    C2D_Sprite spr;
    float dx, dy; // velocity
} Sprite;

extern C2D_SpriteSheet spriteSheet;
extern C3D_RenderTarget* top;
extern C3D_RenderTarget* bottom;
extern C2D_ImageTint* GreyFilter;
extern C2D_ImageTint* GreenFilter;

void InitGFX(void);
void ExitGFX(void);
void DrawSprite(size_t imgindex, float x, float y, const C2D_ImageTint *tint = nullptr, float scaleX = 1.0, float scaleY = 1.0);
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

/* Texture IDs */;

//Main Menu
#define TOWN_ICON           1
#define ACRES_ICON          2
#define PROFILE_ICON        3
#define VILLAGERS_ICON      4
#define PWP_ICON            5
#define ISLAND_ICON         6
#define MAIN_STREET_ICON    7
#define ABOUT_ICON          8
#define BUTTON_MAIN         9
//Game Select Menu
#define EUR_FLAG            10
#define USA_FLAG            11
#define JPN_FLAG            12
#define KOR_FLAG            13
#define ACNL_ICON           14
#define ACNL_WA_ICON        15
#define GAME_CART			35
#define SD_CARD				36
//Town Menu
//Players Menu
#define PLAYER1_PIC         16
#define PLAYER2_PIC         17
#define PLAYER3_PIC         18
#define PLAYER4_PIC         19
#define NO_TPC_PIC          20
#define PLYR_ABOUT          26
#define PLYR_INVENTORY      27
#define PLYR_APPEARANCE     28
#define PLYR_HOUSE          29
#define PLYR_PATTERN        30
#define PLYR_MAILBOX        31
#define PLYR_MAILBOX_UNREAD 32

//Pwps Menu
//Main Street Menu
//Acres Menu
//Villagers Menu
//Island Menu

//About Menu
#define TWITTER_ICON        33
#define DISCORD_ICON        34

//Others
#define NLTK_ICON           21
#define CHECKBOX_EMPTY      22
#define CHECKBOX_FILLED     23
#define CURSOR_POINT        24
#define CURSOR_SELECT       25
#define EDITOR_ICON         37
#define MANAGER_ICON        38

/* Item Icons */
#define ITEM_HOLE           450
#define ITEM_SHEET          451

/* Reserve each acre pic*/
#define ACRE_PNG            500
#define LAST_ACRE_PIC       714


#endif