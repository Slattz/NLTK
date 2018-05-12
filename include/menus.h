#pragma once

#ifndef MENUS_H
#define MENUS_H

#include "common.h"

class ImageButton;
class Button;
class Save;

typedef enum
{
    MODE_EXIT = 0,
    MODE_GAMESELECT = 1,
    MODE_MAINMENU = 2,
    MODE_EDITOR = 3,
    MODE_MANAGER = 4
} ReturnModes;

void checkIfCardInserted(void);
int spawn_main_menu(void);
u64 spawn_game_select_menu(FS_MediaType *mediaType);
void spawn_about_menu(void);
void spawn_config_menu(void);

#endif