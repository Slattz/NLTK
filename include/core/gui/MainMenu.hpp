#pragma once

#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "menus.h"

class ImageButton;
class Button;

namespace Core
{
    ReturnMode Spawn_MainMenu(void);
    void Draw_MainMenu(void);

    namespace MainMenu {
        void Initialize(void);
        void Cleanup(void);
    }
}

#endif