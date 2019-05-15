#pragma once

#ifndef GFX_H
#define GFX_H

#include <string>
#include <citro2d.h>
#include "SpriteIDs.h"

typedef struct
{
    C2D_Sprite spr;
    float dx, dy; // velocity
} Sprite;

enum MsgType {
    MsgTypeNone = 0,
    MsgTypeAccept,
    MsgTypeConfirm
};

extern C3D_RenderTarget* top;
extern C3D_RenderTarget* bottom;
extern C2D_ImageTint* GreyFilter;
extern C2D_ImageTint* GreySelectFilter;
extern C2D_ImageTint* GreenFilter;

extern C2D_SpriteSheet About_ss;
extern C2D_SpriteSheet Acres_ss;
extern C2D_SpriteSheet Common_ss;
extern C2D_SpriteSheet GameSelect_ss;
extern C2D_SpriteSheet Editor_ss;
extern C2D_SpriteSheet Items_ss;
extern C2D_SpriteSheet Players_ss;
extern C2D_SpriteSheet Swkbd_ss;
extern C2D_SpriteSheet Villagers_ss;
extern C2D_SpriteSheet Villagers2_ss;

//Colors
extern const u32 COLOR_BG;  
extern const u32 COLOR_MENU_BARS; 
extern const u32 COLOR_WHITE;   
extern const u32 COLOR_BLACK;
extern const u32 COLOR_GREEN;   
extern const u32 COLOR_BROWN;
extern const u32 COLOR_LIGHT_BROWN;
extern const u32 COLOR_GREY_FILTER;
extern const u32 COLOR_GREY;   
extern const u32 COLOR_DARK_GREY;   
extern const u32 COLOR_RED;
extern const u32 COLOR_YELLOW; 
extern const u32 COLOR_ORANGE; 
extern const u32 COLOR_TRANSPARENT;

/* Image IDs */;

//Main Menu
#define TOWN_ICON           Editor_ss_Town_idx
#define ACRES_ICON          Editor_ss_Acres_idx
#define PROFILE_ICON        Editor_ss_Profile_idx
#define VILLAGERS_ICON      Editor_ss_Villagers_idx
#define PWP_ICON            Editor_ss_Pwp_idx
#define ISLAND_ICON         Editor_ss_Island_idx
#define SHOPS_ICON          Editor_ss_Shops_idx
#define ABOUT_ICON          Editor_ss_About_idx
#define BUTTON_MAIN         Editor_ss_Button_idx
//Game Select Menu
#define EUR_FLAG            GameSelect_ss_Europe_idx
#define USA_FLAG            GameSelect_ss_USA_idx
#define JPN_FLAG            GameSelect_ss_Japan_idx
#define KOR_FLAG            GameSelect_ss_Korea_idx
#define ACNL_ICON           GameSelect_ss_Icon_idx
#define ACNL_WA_ICON        GameSelect_ss_IconWA_idx
#define GAME_CART			GameSelect_ss_GameCart_idx
#define SD_CARD				GameSelect_ss_SDCard_idx
//Town Menu
//Players Menu
#define NO_TPC_PIC          Players_ss_NoTPC_idx
#define PLYR_ABOUT          Players_ss_About_idx
#define PLYR_INVENTORY      Players_ss_Inventory_idx
#define PLYR_APPEARANCE     Players_ss_Appearance_idx
#define PLYR_HOUSE          Players_ss_House_idx
#define PLYR_PATTERN        Players_ss_Patterns_idx
#define PLYR_MAILBOX        Players_ss_Mailbox_idx
#define PLYR_MAILBOX_UNREAD Players_ss_Mailbox_Unread_idx

//Pwps Menu
//Main Street Menu
//Acres Menu
//Villagers Menu
//Island Menu

//About Menu
#define TWITTER_ICON        About_ss_Twitter_idx
#define DISCORD_ICON        About_ss_Discord_idx

//Others
#define NLTK_ICON           Common_ss_icon_idx
#define CHECKBOX_EMPTY      Common_ss_checkbox_idx
#define CHECKBOX_FILLED     Common_ss_checkbox_c_idx
#define EDIT_ICON           Common_ss_edit_idx
#define CURSOR_POINT        Common_ss_hand_point_idx
#define CURSOR_SELECT       Common_ss_hand_select_idx
#define EDITOR_ICON         Common_ss_Editor_idx
#define MANAGER_ICON        Common_ss_Manager_idx

//SWKBD
#define SWKBD_CONFIRM       Swkbd_ss_confirm_idx
#define SWKBD_CANCEL        Swkbd_ss_cancel_idx
#define SWKBD_BACK          Swkbd_ss_back_idx
#define SWKBD_ENTER         Swkbd_ss_enter_idx
#define SWKBD_SHIFT_ON      Swkbd_ss_shift_on_idx
#define SWKBD_SHIFT_OFF     Swkbd_ss_shift_off_idx

/* Item Icons */
#define ITEM_HOLE           Common_ss_ItemHole_idx

/* Reserve each acre pic*/
#define ACRE_PNG            Acres_ss_acre_0_idx
#define LAST_ACRE_PIC       Acres_ss_acre_214_idx


void InitGFX(void);
void ExitGFX(void);
void DrawSprite(C2D_SpriteSheet sheet, size_t imgindex, float x, float y, const C2D_ImageTint *tint = nullptr, float scaleX = 1.0f, float scaleY = 1.0f, float z = 0.f);
void DrawText(float x, float y, float scaleX, float scaleY, u32 color, const char* text);
bool MsgDisp(C3D_RenderTarget *target, std::string message, MsgType Type = MsgTypeAccept, u32 bgColor = COLOR_BROWN, u32 textColor = COLOR_GREY, float textsize = 0.8f, float x = 50.f, float y = 33.5f, float width = 300.f, float height = 180.f);
void DisplayCardError();

void draw_base_interface(void);
void draw_main_menu(void);

#endif