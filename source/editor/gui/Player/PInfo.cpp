#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "CTRFont.hpp"
#include "textbox.h"
#include "label.h"
#include "gfx.h"
#include "save.h"
#include "e_utils.h"
#include "utils.h"
#include "InputManager.h"
#include "menus.h"
#include "gui/PlayerMenu.hpp"

std::vector<Control*> editorPlayerInfoControls = {};

TextBox playerNameBox;
TextBox playerWalletBox;
TextBox playerSavingsBox;
TextBox playerMedalsBox;
TextBox playerCouponsBox;

Label playerNameLabel;
Label playerWalletLabel;
Label playerSavingsLabel;
Label playerMedalsLabel;
Label playerCouponsLabel;

void Editor::Player::InitInfoGFX(void)
{
    // Create Controls
    playerNameLabel = Label(10, 10, 58, 16, COLOR_TRANSPARENT, COLOR_WHITE, "Name:");
    playerNameBox = TextBox(68, 10, 100, 16, "Name", "Enter a name for the player.", 8, KeyboardInType::All, COLOR_WHITE, COLOR_BLACK);
    playerWalletLabel = Label(10, 30, 58, 16, COLOR_TRANSPARENT, COLOR_WHITE, "Wallet:");
    playerWalletBox = TextBox(68, 30, 100, 16, "0", "Enter wallet amount.", 5, KeyboardInType::Numbers, COLOR_WHITE, COLOR_BLACK);
    playerSavingsLabel = Label(10, 50, 58, 16, COLOR_TRANSPARENT, COLOR_WHITE, "Savings:");
    playerSavingsBox = TextBox(68, 50, 100, 16, "0", "Enter savings amount.", 9, KeyboardInType::Numbers, COLOR_WHITE, COLOR_BLACK);
    playerMedalsLabel = Label(10, 70, 58, 16, COLOR_TRANSPARENT, COLOR_WHITE, "Medals:");
    playerMedalsBox = TextBox(68, 70, 100, 16, "0", "Enter island medals amount.", 5, KeyboardInType::Numbers, COLOR_WHITE, COLOR_BLACK);
    playerCouponsLabel = Label(10, 90, 58, 16, COLOR_TRANSPARENT, COLOR_WHITE, "Coupons:");
    playerCouponsBox = TextBox(68, 90, 100, 16, "0", "Enter meow coupons amount.", 5, KeyboardInType::Numbers, COLOR_WHITE, COLOR_BLACK);

    editorPlayerInfoControls.push_back(&playerNameBox);
    editorPlayerInfoControls.push_back(&playerWalletBox);
    editorPlayerInfoControls.push_back(&playerSavingsBox);
    editorPlayerInfoControls.push_back(&playerMedalsBox);
    editorPlayerInfoControls.push_back(&playerCouponsBox);

    editorPlayerInfoControls.push_back(&playerNameLabel);
    editorPlayerInfoControls.push_back(&playerWalletLabel);
    editorPlayerInfoControls.push_back(&playerSavingsLabel);
    editorPlayerInfoControls.push_back(&playerMedalsLabel);
    editorPlayerInfoControls.push_back(&playerCouponsLabel);
}

static void Draw_PlayerMenu_Info(int selectedPlayer) {
    Editor::Player::Draw_PlayerMenuTop(selectedPlayer);
    C2D_SceneBegin(bottom);

    // Draw Controls
    for (Control *c : editorPlayerInfoControls) {
        c->Draw();
    }

    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}


static void refreshInfoMenu() {
    playerNameBox.myText = u16tou8(Save::Instance()->players[EditorConfig.SelectedPlayer]->Name); // TODO: Change player name to utf8? Or at least create a utf8 equivalent
    playerWalletBox.myText = std::to_string(Save::Instance()->players[EditorConfig.SelectedPlayer]->Wallet.value);
    playerSavingsBox.myText = std::to_string(Save::Instance()->players[EditorConfig.SelectedPlayer]->Savings.value);
    playerMedalsBox.myText = std::to_string(Save::Instance()->players[EditorConfig.SelectedPlayer]->IslandMedals.value);
    playerCouponsBox.myText = std::to_string(Save::Instance()->players[EditorConfig.SelectedPlayer]->MeowCoupons.value);
}

void Editor::Player::Spawn_PlayerMenu_Info() {
    if (EditorConfig.DrawingSubmenu)
        return;

    EditorConfig.DrawingSubmenu = true;

    // Set initial textbox values
    refreshInfoMenu();

    while (aptMainLoop()) {
        checkIfCardInserted();

        Draw_PlayerMenu_Info(EditorConfig.SelectedPlayer);
        InputManager::Instance()->RefreshInput();

        EditorConfig.LColor = EditorConfig.RColor = COLOR_GREY;

        if (InputManager::Instance()->IsButtonActive(KEY_B))
            break;

        if (InputManager::Instance()->IsButtonHeld(KEY_R))
            EditorConfig.RColor = COLOR_WHITE;

        if (InputManager::Instance()->IsButtonHeld(KEY_L))
            EditorConfig.LColor = COLOR_WHITE;

        if (InputManager::Instance()->IsButtonDown(KEY_R))
        {
            while (true) {
                EditorConfig.SelectedPlayer++;
                if (EditorConfig.SelectedPlayer > 3)
                    EditorConfig.SelectedPlayer = 0;

                if (Save::Instance()->players[EditorConfig.SelectedPlayer]->Exists()) {
                    break;
                }
            }

            refreshInfoMenu();
        }

        if (InputManager::Instance()->IsButtonDown(KEY_L))
        {
            while (true) {
                EditorConfig.SelectedPlayer--;
                if (EditorConfig.SelectedPlayer < 0)
                    EditorConfig.SelectedPlayer = 3;

                if (Save::Instance()->players[EditorConfig.SelectedPlayer]->Exists()) {
                    break;
                }
            }

            refreshInfoMenu();
        }

        // Check for input in the info menu
        if (playerNameBox.IsActive()) { // Player Name Box
            // Find all references to the Player's id/name
            std::vector<u32> m_PlayerIdReferences = findPlayerReferences(Save::Instance()->players[EditorConfig.SelectedPlayer]);
            Save::Instance()->players[EditorConfig.SelectedPlayer]->Name = u8tou16(playerNameBox.Activate().c_str());

            // Replace all references to Player's id/name
            for (u32 offset : m_PlayerIdReferences) {
                Save::Instance()->Write(offset, Save::Instance()->players[EditorConfig.SelectedPlayer]->PlayerId);
                Save::Instance()->Write(offset + 2, Save::Instance()->players[EditorConfig.SelectedPlayer]->Name, 8);
            }

            Save::Instance()->SetChangesMade(true);
        }

        else if (playerWalletBox.IsActive()) {
            Save::Instance()->players[EditorConfig.SelectedPlayer]->Wallet.value = static_cast<u32>(std::stoi(playerWalletBox.Activate()));
            Save::Instance()->SetChangesMade(true);
        }

        else if (playerSavingsBox.IsActive()) {
            Save::Instance()->players[EditorConfig.SelectedPlayer]->Savings.value = static_cast<u32>(std::stoi(playerSavingsBox.Activate()));
            Save::Instance()->SetChangesMade(true);
        }

        else if (playerMedalsBox.IsActive()) {
            Save::Instance()->players[EditorConfig.SelectedPlayer]->IslandMedals.value = static_cast<u32>(std::stoi(playerMedalsBox.Activate()));
            Save::Instance()->SetChangesMade(true);
        }
                
        else if (playerCouponsBox.IsActive()) {
            Save::Instance()->players[EditorConfig.SelectedPlayer]->MeowCoupons.value = static_cast<u32>(std::stoi(playerCouponsBox.Activate()));
            Save::Instance()->SetChangesMade(true);
        }
    }

    EditorConfig.DrawingSubmenu = false;
}