#pragma once

#ifndef EDITORMENU_HPP
#define EDITORMENU_HPP

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