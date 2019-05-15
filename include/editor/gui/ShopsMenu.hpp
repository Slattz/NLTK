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

    }

} // namespace Editor

#endif