#include <map>
#include "gfx.h"
#include "menus.h"
#include "save.h"
#include "gui/VillagerMenu.hpp"

extern std::map<u16, std::string> g_villagerDatabase;

static std::vector<ImageButton *> m_villagerEditorButtons;
static Text *m_villagerNameText = nullptr;
static Villager *m_currentlySelectedVillager = nullptr;
bool selectingNewVillager = false;
u16 newVillagerId = 0xFFFF;

void VillagerEditor::Initialize() {
    // Initialize controls
    u32 idx = 0;
    for (u32 y = 0; y < 2; y++) {
        for (u32 x = 0; x < 5; x++, idx++) { 
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
    for (const auto button : m_villagerEditorButtons) {
        delete button;
    }

    m_villagerEditorButtons.clear();
    if (m_villagerNameText != nullptr) {
        delete m_villagerNameText;
        m_villagerNameText = nullptr;
    }

    m_currentlySelectedVillager = nullptr; //This shouldn't be deleted as it'll then delete the actual Villager
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
    if (selectingNewVillager) {
        DrawVillagerSprite(newVillagerId, {174, 66}, 1.0f, 1.0f);
        m_villagerNameText->Draw();
    }

    C2D_SceneBegin(bottom);

    // Draw villager selector buttons
    for (const auto button : m_villagerEditorButtons) {
        button->Draw();
    }

    // Draw the next and previous (if possible) villager.
    if (m_currentlySelectedVillager != nullptr && selectingNewVillager) {
        // Get the index of the villager so we can determine the current button to display the info on.
        const u32 idx = m_currentlySelectedVillager->GetIndex();

        // Draw previous first
        if (newVillagerId == 0xFFFF) {
            DrawVillagerSprite(398, { 43 + (idx % 5) * 58, 40 + (idx / 5) * 58 }, 0.5f, 0.5);
        }

        else if (newVillagerId <= 398) {
            DrawVillagerSprite(newVillagerId == 0 ? 0xFFFF : newVillagerId - 1, { 43 + (idx % 5) * 58, 40 + (idx / 5) * 58 }, 0.5f, 0.5);
        }


        // Draw next
        if (newVillagerId <= 398 || newVillagerId == 0xFFFF) {
            DrawVillagerSprite(newVillagerId == 0xFFFF ? 0 : newVillagerId + 1, { 43 + (idx % 5) * 58, 109 + (idx / 5) * 58 }, 0.5f, 0.5);
        }
    }

    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

bool VillagerEditor::ProcessInput() {
    bool ignoreB = false;

    // Check activated villager buttons.
    // Skip this if a villager is currently being edited.
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
            m_currentlySelectedVillager->SetId(newVillagerId);
            Save::Instance()->SetChangesMade(true);
        }

        else if (InputManager::Instance()->IsButtonDown(KEY_B)) {
            selectingNewVillager = false;
            //Reset Villager ID
            newVillagerId = m_currentlySelectedVillager->GetId();
            u32 clampedIdx = newVillagerId > 399 ? 399 : newVillagerId;
            //Reset Villager Pic
            m_villagerEditorButtons[idx]->SetSpriteSheet(clampedIdx < 200 ? Villagers_ss : Villagers2_ss);
            m_villagerEditorButtons[idx]->SetImageId(clampedIdx < 200 ? clampedIdx : clampedIdx - 200);
            ignoreB = true;
        }

        else if (InputManager::Instance()->IsButtonDown(KEY_DDOWN)) {
            if (newVillagerId < 398) {
                newVillagerId++;
            }
            else if (newVillagerId == 0xFFFF) { // No villager case
                newVillagerId = 0;
            }

            else if (newVillagerId >= 398) newVillagerId = 0xFFFF;


            u32 clampedIdx = newVillagerId > 399 ? 399 : newVillagerId;

            m_villagerEditorButtons[idx]->SetSpriteSheet(clampedIdx < 200 ? Villagers_ss : Villagers2_ss);
            m_villagerEditorButtons[idx]->SetImageId(clampedIdx < 200 ? clampedIdx : clampedIdx - 200);
            *m_villagerNameText = g_villagerDatabase[clampedIdx];
            m_villagerNameText->SetPos((412 - m_villagerNameText->GetWidth()) / 2, 126);
        }
        
        else if (InputManager::Instance()->IsButtonDown(KEY_DUP)) {
            if (newVillagerId == 0) {
                newVillagerId = 0xFFFF;
            }
            else if (newVillagerId == 0xFFFF) { //No villager case
                newVillagerId = 398;
            }

            else if (newVillagerId > 0 && newVillagerId != 0xFFFF) {
                newVillagerId--;
            }

            u32 clampedIdx = newVillagerId > 399 ? 399 : newVillagerId;

            m_villagerEditorButtons[idx]->SetSpriteSheet(clampedIdx < 200 ? Villagers_ss : Villagers2_ss);
            m_villagerEditorButtons[idx]->SetImageId(clampedIdx < 200 ? clampedIdx : clampedIdx - 200);
            *m_villagerNameText = g_villagerDatabase[newVillagerId];
            m_villagerNameText->SetPos((412 - m_villagerNameText->GetWidth()) / 2, 126);
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
    }

    // Cleanup.
    VillagerEditor::Cleanup();
}