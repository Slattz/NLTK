#pragma once

#ifndef SHOPSMENU_HPP
#define SHOPSMENU_HPP

#include "save.h"

struct ShopsSettings {
    bool DrawingMenu = false;
    bool DrawingSubmenu = false;
};

extern ShopsSettings ShopsConfig;

namespace Editor
{
    void Spawn_ShopsMenu();
    void Draw_ShopsMenu();

    namespace Shops
    {
        void InitInfoGFX(void);
        void Spawn_PlayerMenu_Info();
        void Spawn_PlayerMenu_Inventory();
        void Spawn_PlayerMenu_Patterns();
    }

} // namespace Editor

#endif