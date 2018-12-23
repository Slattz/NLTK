#include <map>
#include "gfx.h"
#include "menus.h"
#include "save.h"
#include "gui/VillagerMenu.hpp"

extern std::map<u16, std::string> g_villagerDatabase;

void VillagerEditor::Initialize() {
    // Initialize controls
    int idx = 0;
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 5; x++, idx++) { 
            u16 villagerId = Save::Instance()->villagers[idx]->GetId();
            if (villagerId > 399) {
                villagerId = 399;
            }

            ImageButton* villagerEditorButton = new ImageButton({ 15 + x * 58, 67 + y * 58 }, { 48, 48 }, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH,
                villagerId < 200 ? villagerId : villagerId - 200, villagerId < 200 ? Villagers_ss : Villagers2_ss);
            
            villagerEditorButton->SetScale(0.75f);
            m_villagerEditorButtons.push_back(villagerEditorButton);
        }
    }

    m_villagerNameText = new Text(COLOR_WHITE, "", 1.0f, 1.0f);
}

void VillagerEditor::Cleanup() {
    for (auto button : m_villagerEditorButtons) {
        delete button;
    }

    m_villagerEditorButtons.clear();
    delete m_villagerNameText;
}

void VillagerEditor::Draw() {
    draw_base_interface();

    // Draw the currently select villager's portrait in higher resolution on the top screen
    if (m_currentlySelectedVillager != nullptr) {
        u16 id = m_currentlySelectedVillager->GetId();
        if (id > 399) {
            id = 399;
        }

        DrawSprite(id < 200 ? Villagers_ss : Villagers2_ss, id < 200 ? id : id - 200, 174, 66);
        m_villagerNameText->Draw();
    }

    C2D_SceneBegin(bottom);

    // Draw villager selector buttons
    for (auto button : m_villagerEditorButtons) {
        button->Draw();
    }

    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

void VillagerEditor::ProcessInput() {
    int idx = 0;
    for (auto villagerButton = m_villagerEditorButtons.begin(); villagerButton != m_villagerEditorButtons.end(); villagerButton++, idx++) {
        if ((*villagerButton)->IsActive()) {
            m_currentlySelectedVillager = Save::Instance()->villagers[idx];
            *m_villagerNameText = g_villagerDatabase[m_currentlySelectedVillager->GetId()];
            m_villagerNameText->SetPos((412 - m_villagerNameText->GetWidth()) / 2, 126);
            break;
        }
    }
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
        VillagerEditor::ProcessInput();
        InputManager::Instance()->DrawCursor();
    }

    // Cleanup.
    VillagerEditor::Cleanup();
}