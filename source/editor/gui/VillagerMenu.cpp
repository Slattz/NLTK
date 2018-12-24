#include <map>
#include "gfx.h"
#include "menus.h"
#include "save.h"
#include "gui/VillagerMenu.hpp"

extern std::map<u16, std::string> g_villagerDatabase;

bool selectingNewVillager = false;
u16 newVillagerId = 0xFFFF;

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
    m_currentlySelectedVillager = nullptr;
    selectingNewVillager = false;
    newVillagerId = 0xFFFF;
}

void VillagerEditor::DrawVillagerSprite(u16 villagerId, const Point_t position, const float sizeX, const float sizeY) {
    if (villagerId > 399) {
        villagerId = 399;
    }

    if (villagerId < 200) {
        DrawSprite(Villagers_ss, villagerId, position.X, position.Y, nullptr, sizeX, sizeY);
    }
    else {
        DrawSprite(Villagers2_ss, villagerId - 200, position.X, position.Y, nullptr, sizeX, sizeY);
    }
}

void VillagerEditor::Draw() {
    draw_base_interface();

    // Draw the currently select villager's portrait in higher resolution on the top screen
    if (m_currentlySelectedVillager != nullptr) {
        DrawVillagerSprite(m_currentlySelectedVillager->GetId(), { 174, 66 }, 1.0f, 1.0f);
        m_villagerNameText->Draw();
    }

    C2D_SceneBegin(bottom);

    // Draw villager selector buttons
    for (auto button : m_villagerEditorButtons) {
        button->Draw();
    }

    // Draw the next and previous (if possible) villager.
    if (m_currentlySelectedVillager != nullptr && selectingNewVillager) {
        // Get the index of the villager so we can determine the current button to display the info on.
        const u32 idx = m_currentlySelectedVillager->GetIndex();

        // Draw previous first
        if (newVillagerId != 0xFFFF) {
            DrawVillagerSprite(newVillagerId == 0 ? 0xFFFF : newVillagerId - 1, { 43 + (idx % 5) * 58, 40 + (idx / 5) * 58 }, 0.5f, 0.5);
        }

        // Draw next
        if (newVillagerId < 0x398 || newVillagerId == 0xFFFF) {
            DrawVillagerSprite(newVillagerId == 0xFFFF ? 0 : newVillagerId + 1, { 43 + (idx % 5) * 58, 109 + (idx / 5) * 58 }, 0.5f, 0.5);
        }
    }

    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

bool VillagerEditor::ProcessInput() {
    bool ignoreB = false;

    // Draw villager buttons.
    // Skip drawing if one is currently being edited.
    if (!selectingNewVillager) {
        int idx = 0;
        for (auto villagerButton = m_villagerEditorButtons.begin(); villagerButton != m_villagerEditorButtons.end(); villagerButton++, idx++) {
            if ((*villagerButton)->IsDown()) {
                m_currentlySelectedVillager = Save::Instance()->villagers[idx];
                *m_villagerNameText = g_villagerDatabase[m_currentlySelectedVillager->GetId()];
                m_villagerNameText->SetPos((412 - m_villagerNameText->GetWidth()) / 2, 126);
                selectingNewVillager = true;
                newVillagerId = m_currentlySelectedVillager->GetId();
                break;
            }
        }
    }

    // Process the villager being edited.
    // TODO: I think another screen with a scrollable list of villagers would be a better solution.
    // This solution is just temporary to get functionality implemented ASAP.
    else if (m_currentlySelectedVillager != nullptr && selectingNewVillager) {
        const u32 idx = m_currentlySelectedVillager->GetIndex();

        if (InputManager::Instance()->IsButtonDown(KEY_A)) {
            selectingNewVillager = false;
        }
        else if (InputManager::Instance()->IsButtonDown(KEY_B)) {
            selectingNewVillager = false;
            newVillagerId = 0xFFFF;
            ignoreB = true;
        }
        else if (InputManager::Instance()->IsButtonDown(KEY_DDOWN)) {
            if (newVillagerId < 398) {
                newVillagerId++;
            }
            else if (newVillagerId == 0xFFFF) { // No villager case
                newVillagerId = 0;
            }

            m_villagerEditorButtons[idx]->SetSpriteSheet(newVillagerId < 200 ? Villagers_ss : Villagers2_ss);
            m_villagerEditorButtons[idx]->SetImageId(newVillagerId < 200 ? newVillagerId : newVillagerId - 200);
            *m_villagerNameText = g_villagerDatabase[newVillagerId];
            m_villagerNameText->SetPos((412 - m_villagerNameText->GetWidth()) / 2, 126);
            m_currentlySelectedVillager->SetId(newVillagerId);
        }
        else if (InputManager::Instance()->IsButtonDown(KEY_DUP)) {
            if (newVillagerId == 0) {
                newVillagerId = 0xFFFF;
            }
            else if (newVillagerId > 0 && newVillagerId != 0xFFFF) {
                newVillagerId--;
            }

            u32 clampedIdx = newVillagerId > 0x399 ? 399 : newVillagerId;

            m_villagerEditorButtons[idx]->SetSpriteSheet(clampedIdx < 200 ? Villagers_ss : Villagers2_ss);
            m_villagerEditorButtons[idx]->SetImageId(clampedIdx < 200 ? clampedIdx : clampedIdx - 200);
            *m_villagerNameText = g_villagerDatabase[newVillagerId];
            m_villagerNameText->SetPos((412 - m_villagerNameText->GetWidth()) / 2, 126);
            m_currentlySelectedVillager->SetId(newVillagerId);
        }
    }

    return ignoreB;
}

void VillagerEditor::Spawn() {
    // Cleanup any unfreed resources.
    VillagerEditor::Cleanup();

    // Initialize resources.
    VillagerEditor::Initialize();

    while (aptMainLoop()) {
        InputManager::Instance()->RefreshInput();
        checkIfCardInserted();

        VillagerEditor::Draw();

        if (!VillagerEditor::ProcessInput() && InputManager::Instance()->IsButtonDown(KEY_B)) {
            break;
        }

        InputManager::Instance()->DrawCursor();
    }

    // Cleanup.
    VillagerEditor::Cleanup();
}