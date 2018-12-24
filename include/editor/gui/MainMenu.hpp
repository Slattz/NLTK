#pragma once

#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "menus.h"

namespace Editor
{
    bool SaveCheck(void);
    void Draw_MainMenu(void);
    void Init_MainMenu(void);
    ReturnMode Spawn_MainMenu(void);
} // namespace Editor

#endif