#pragma once

#ifndef PLAYERMENU_HPP
#define PLAYERMENU_HPP

#include "save.h"

struct EditorSettings {
    s32  SelectedPlayer = 0;
    bool DrawingMenu = false;
    bool DrawingSubmenu = false;
    u32 LColor = COLOR_GREY;
    u32 RColor = COLOR_GREY;
};

extern EditorSettings EditorConfig;

    namespace Editor
{
    void Spawn_PlayerMenu(Save *saveFile);
    void Draw_PlayerMenu(Save *saveFile, int selectedplayer, int selectedmode);

    namespace Player
    {
        void InitInfoGFX(void);
        void Draw_PlayerMenuTop(Save *saveFile, int selectedplayer);
        void Spawn_PlayerMenu_Info(Save *saveFile);
        void Spawn_PlayerMenu_Inventory(Save *saveFile);
        void Spawn_PlayerMenu_Patterns(Save *saveFile);
    }

} // namespace Editor

#endif