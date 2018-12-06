#pragma once

#ifndef ACREMENU_HPP
#define ACREMENU_HPP

#include "save.h"

struct AcreSettings {
    s32  SelectedPlayer = 0;
    s32  SelectedAcre = -1;
    bool DrawingMenu = false;
    bool DrawingSubmenu = false;
};

extern AcreSettings AcreConfig;

namespace Editor
{
    void Spawn_AcresMenu();

    namespace Acre
    {
        void InitAcreGFX(const u8 LoopMax, const u8 GridXMax, 
            const u8 GridXStartPos, const u8 GridYStartPos, const u8 ByteSkip, u32 Offset);
    }

} // namespace Editor

#endif