#pragma once

#include <vector>
#include "imagebutton.h"

namespace VillagerEditor {
    void Initialize();
    void Cleanup();
    void Draw();
    void Spawn();

    static std::vector<ImageButton*> m_villagerEditorButtons;
}