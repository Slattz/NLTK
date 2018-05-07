#pragma once

#ifndef GFX_H
#define GFX_H

#include "common.h"

void InitCommonGFX(void);
void ExitCommonGFX(void);
void infoDisp(gfxScreen_t target, const char* message);
bool confirmDisp(gfxScreen_t target, const char* message);
void DisplayText(gfxScreen_t target, int x, int y, int width, int height, float textsize, const char* message);
void infoDispF(gfxScreen_t target, const char* string, ...);
void confirmDispF(gfxScreen_t target, const char* string, ...);
void DisplayTextF(gfxScreen_t target, int x, int y, int width, int height, float textsize, const char* string, ...);
void DisplayCardError();

void draw_base_interface(void);
void draw_cursor(void);
void draw_game_select_menu(int selectedgame, int selectedregion, int selectedmedia);
void draw_about_menu(bool discord, bool twitter);
void draw_config_menu(void);

//Colors
//#define COLOR_BOTTOMBACKGROUND  ABGR8(255,142,255,124) //Light Green
//#define COLOR_TOPBACKGROUND     ABGR8(255,45,78,80) //Brown
#define COLOR_GREENBACKGROUND   ABGR8(255,105,188,92)
#define COLOR_MENU_BARS         RGBA8(33,139,43,255)
#define COLOR_WHITE             RGBA8(255,255,255,255)
#define COLOR_BLACK             RGBA8(0,0,0,255)
#define COLOR_GREEN             RGBA8(124,255,142,255)
#define COLOR_BROWN             RGBA8(80,78,45,255)
#define COLOR_LIGHT_BROWN       RGBA8(99,93,31,255)
#define COLOR_GREY_FILTER       RGBA8(0,0,0,170)
#define COLOR_GREY              RGBA8(210,210,210,255)
#define COLOR_RED               RGBA8(255,0,0,255)
#define COLOR_YELLOW			RGBA8(255, 255, 0, 255)
#define COLOR_ORANGE            RGBA8(255,106,0,255)
#define COLOR_TRANSPARENT		RGBA8(0, 0, 0, 0)

/* Texture IDs */

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
#define GAME_CART			98
#define SD_CARD				99
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

/* Item Icons */
#define ITEM_HOLE           450
#define ITEM_SHEET          451

/* Reserve each acre pic*/
#define ACRE_PNG            500
#define LAST_ACRE_PIC       714


#endif