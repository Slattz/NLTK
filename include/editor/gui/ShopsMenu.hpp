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
    void Spawn_ShopsMenu(Save *saveFile);
    void Draw_ShopsMenu(Save *saveFile);

    namespace Shops
    {
        void InitInfoGFX(void);
        void Spawn_PlayerMenu_Info(Save *saveFile);
        void Spawn_PlayerMenu_Inventory(Save *saveFile);
        void Spawn_PlayerMenu_Patterns(Save *saveFile);
    }

} // namespace Editor

#endif