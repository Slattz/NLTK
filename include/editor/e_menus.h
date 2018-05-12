#pragma once

#ifndef E_MENUS_H
#define E_MENUS_H

#include "common.h"

class ImageButton;
class Button;
class Save;

extern s32 currentAcreId;

bool check_if_saving(void);
int spawn_editor_main_menu(Save *saveFile);
void spawn_player_menu(Save *saveFile);
void spawn_player_menu_info(Save *saveFile);
void spawn_player_menu_inventory(Save *saveFile);
void view_acres_town_full(Save *saveFile);
void view_acres_island_full(Save *saveFile);
void view_acres_town_map(Save *saveFile);
void view_acres_island_map(Save *saveFile);
void spawn_acres_menu(Save *saveFile);
void onAcreClick(Button *sender);

#endif