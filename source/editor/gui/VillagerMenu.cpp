#include "gfx.h"
#include "menus.h"
#include "save.h"
#include "gui/VillagerMenu.hpp"

void VillagerEditor::Initialize() {
    // Initialize controls
    int idx = 0;
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 5; x++, idx++) { 
            u16 villagerId = Save::Instance()->villagers[idx]->GetId();
            ImageButton* villagerEditorButton = new ImageButton({ 15 + x * 58, 67 + y * 58 }, { 48, 48 }, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH,
                villagerId < 399 ? villagerId : 399, Villagers_ss);

            m_villagerEditorButtons.push_back(villagerEditorButton);
        }
    }
}

void VillagerEditor::Cleanup() {
    for (auto button : m_villagerEditorButtons) {
        delete button;
    }

    m_villagerEditorButtons.clear();
}

void VillagerEditor::Draw() {
    draw_base_interface();
    
    C2D_SceneBegin(bottom);

    for (auto button : m_villagerEditorButtons) {
        button->Draw();
    }

    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

void VillagerEditor::Spawn() {
    // Cleanup any unfreed resources.
    VillagerEditor::Cleanup();

    // Initialize resources.
    VillagerEditor::Initialize();

    while (aptMainLoop()) {
        InputManager::Instance()->RefreshInput();
        checkIfCardInserted();

        if (InputManager::Instance()->IsButtonActive(KEY_B)) {
            break;
        }

        VillagerEditor::Draw();
    }

    // Cleanup.
    VillagerEditor::Cleanup();
}