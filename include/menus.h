#pragma once

#ifndef MENUS_H
#define MENUS_H

#include "common.h"
#include "save.h"

enum class ReturnMode {
    None = -1,
    Exit = 0,
    GameSelect = 1,
    MainMenu = 2,
    Editor = 3,
    Manager = 4
};

void checkIfCardInserted(void);

#endif