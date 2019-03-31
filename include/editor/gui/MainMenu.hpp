#pragma once

#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "menus.h"

namespace Editor
{
    ReturnMode Spawn_MainMenu(void);
    namespace MainMenu 
    {
        void Draw(void);
        bool SaveCheck(void);
        void Initialize(void);
        void Cleanup(void);
    }
} // namespace Editor

#endif