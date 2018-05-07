#pragma once

#ifndef MENUS_H
#define MENUS_H

#include "common.h"

class ImageButton;
class Button;
class Save;

extern s32 currentAcreId;

typedef enum
{
    MODE_EXIT = 0,
    MODE_GAMESELECT = 1,
} Editor_ReturnMode;


bool check_if_saving(void);
int spawn_editor_main_menu(Save *saveFile);
u64 spawn_game_select_menu(FS_MediaType *mediaType);
void spawn_player_menu(Save *saveFile);
void spawn_player_menu_info(Save *saveFile);
void spawn_player_menu_inventory(Save *saveFile);
void spawn_about_menu(void);
void spawn_config_menu(void);
void view_acres_town_full(Save *saveFile);
void view_acres_island_full(Save *saveFile);
void view_acres_town_map(Save *saveFile);
void view_acres_island_map(Save *saveFile);
void spawn_acres_menu(Save *saveFile);
void onAcreClick(Button *sender);

#endif