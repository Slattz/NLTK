#pragma once

#ifndef E_MENUS_H
#define E_MENUS_H

#include "button.h"

extern s32 currentAcreId;

bool check_if_saving(void);
void view_acres_town_full(Save *saveFile);
void view_acres_island_full(Save *saveFile);
void view_acres_town_map(Save *saveFile);
void view_acres_island_map(Save *saveFile);
void spawn_acres_menu(Save *saveFile);
void onAcreClick(Button *sender);

#endif