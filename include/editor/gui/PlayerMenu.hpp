#pragma once

#ifndef PLAYERMENU_HPP
#define PLAYERMENU_HPP

#include "save.h"

struct PlayerSettings {
    s32  SelectedPlayer = 0;
    bool DrawingMenu = false;
    bool DrawingSubmenu = false;
    u32 LColor = COLOR_GREY;
    u32 RColor = COLOR_GREY;
};

extern PlayerSettings EditorConfig;

namespace Editor
{
    void Spawn_PlayerMenu();
    void Draw_PlayerMenu(int selectedplayer, int selectedmode);

    namespace Player
    {
        void InitInfoGFX(void);
        void CleanupInfoGFX(void);
        void Draw_PlayerMenuTop(int selectedplayer);
        void Spawn_PlayerMenu_Info(void);
        void Spawn_PlayerMenu_Inventory(void);
        void Spawn_PlayerMenu_Patterns(void);
    }

} // namespace Editor

#endif