#pragma once

#ifndef PLAYERMENU_HPP
#define PLAYERMENU_HPP

#include "save.h"

struct PlayerSettings {
    s32  SelectedPlayer = 0;
    u32 LColor = COLOR_GREY;
    u32 RColor = COLOR_GREY;
    bool DrawingMenu = false;
    bool DrawingSubmenu = false;
    bool TopMenuInit = false;
};

extern PlayerSettings PlayerConfig;

namespace Editor
{
    void Spawn_PlayerMenu();
    void Draw_PlayerMenu(void);

    namespace Player
    {
        void InitializeTop(void);
        void CleanupTop(void);
        void Initialize(void);
        void Cleanup(void);
        
        void UpdateTopInfo(void);

        void InitInfoGFX(void);
        void CleanupInfoGFX(void);
        void Draw_PlayerMenuTop(void);
        void Spawn_PlayerMenu_Info(void);
        void Spawn_PlayerMenu_Inventory(void);
        void Spawn_PlayerMenu_Patterns(void);
    }

} // namespace Editor

#endif