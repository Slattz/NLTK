#pragma once

#ifndef MENUS_H
#define MENUS_H

#include "common.h"
#include "save.h"

class ImageButton;
class Button;

enum class ReturnMode {
    None = -1,
    Exit = 0,
    GameSelect = 1,
    MainMenu = 2,
    Editor = 3,
    Manager = 4
};

void checkIfCardInserted(void);
ReturnMode spawn_main_menu(void);
u64 spawn_game_select_menu(FS_MediaType *mediaType);
void spawn_about_menu(void);

#endif