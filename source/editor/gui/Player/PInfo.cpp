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
#include "cursor.h"
#include "menus.h"
#include "gui/PlayerMenu.hpp"

extern Cursor g_cursor;
extern s16 g_CheckX[2];
extern s16 g_CheckY[2];
extern bool g_disabled[2];
extern u32 g_key[2];

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
    playerNameBox = TextBox(68, 10, 100, 16, "Name", "Enter a name for the player.", 8, SWKBD_TYPE_NORMAL, COLOR_WHITE, COLOR_BLACK);
    playerWalletLabel = Label(10, 30, 58, 16, COLOR_TRANSPARENT, COLOR_WHITE, "Wallet:");
    playerWalletBox = TextBox(68, 30, 100, 16, "0", "Enter wallet amount.", 5, SWKBD_TYPE_NUMPAD, COLOR_WHITE, COLOR_BLACK);
    playerSavingsLabel = Label(10, 50, 58, 16, COLOR_TRANSPARENT, COLOR_WHITE, "Savings:");
    playerSavingsBox = TextBox(68, 50, 100, 16, "0", "Enter savings amount.", 9, SWKBD_TYPE_NUMPAD, COLOR_WHITE, COLOR_BLACK);
    playerMedalsLabel = Label(10, 70, 58, 16, COLOR_TRANSPARENT, COLOR_WHITE, "Medals:");
    playerMedalsBox = TextBox(68, 70, 100, 16, "0", "Enter island medals amount.", 5, SWKBD_TYPE_NUMPAD, COLOR_WHITE, COLOR_BLACK);
    playerCouponsLabel = Label(10, 90, 58, 16, COLOR_TRANSPARENT, COLOR_WHITE, "Coupons:");
    playerCouponsBox = TextBox(68, 90, 100, 16, "0", "Enter meow coupons amount.", 5, SWKBD_TYPE_NUMPAD, COLOR_WHITE, COLOR_BLACK);

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

static void Draw_PlayerMenu_Info(Save *saveFile, int selectedPlayer) {
    Editor::Player::Draw_PlayerMenuTop(saveFile, selectedPlayer);
    C2D_SceneBegin(bottom);

    // Draw Controls
    for (Control *c : editorPlayerInfoControls) {
        c->Draw();
    }

    g_cursor.Draw();
    C3D_FrameEnd(0);
}


static void refreshInfoMenu(Save *saveFile) {
    playerNameBox.myText = u16tou8(saveFile->players[EditorConfig.SelectedPlayer].Name); // TODO: Change player name to utf8? Or at least create a utf8 equivalent
    playerWalletBox.myText = std::to_string(saveFile->players[EditorConfig.SelectedPlayer].Wallet.value);
    playerSavingsBox.myText = std::to_string(saveFile->players[EditorConfig.SelectedPlayer].Savings.value);
    playerMedalsBox.myText = std::to_string(saveFile->players[EditorConfig.SelectedPlayer].IslandMedals.value);
    playerCouponsBox.myText = std::to_string(saveFile->players[EditorConfig.SelectedPlayer].MeowCoupons.value);
}

void Editor::Player::Spawn_PlayerMenu_Info(Save *saveFile) {
    if (EditorConfig.DrawingSubmenu)
        return;

    EditorConfig.DrawingSubmenu = true;

    u32 m_keysDown, m_keysHeld = 0;

    // Set initial textbox values
    refreshInfoMenu(saveFile);

    while (aptMainLoop()) {
        checkIfCardInserted();

        hidScanInput();
        Draw_PlayerMenu_Info(saveFile, EditorConfig.SelectedPlayer);
        updateCursorInfo();

        m_keysDown = hidKeysDown();
        m_keysHeld = hidKeysHeld();

        EditorConfig.LColor = EditorConfig.RColor = COLOR_GREY;

        if (m_keysDown & KEY_B || m_keysHeld & KEY_B)
            break;

        if (m_keysHeld & KEY_R)
            EditorConfig.RColor = COLOR_WHITE;

        if (m_keysHeld & KEY_L)
            EditorConfig.LColor = COLOR_WHITE;

        if (m_keysDown & KEY_R)
        {
            while (true) {
                EditorConfig.SelectedPlayer++;
                if (EditorConfig.SelectedPlayer > 3)
                    EditorConfig.SelectedPlayer = 0;

                if (saveFile->players[EditorConfig.SelectedPlayer].Exists(saveFile)) {
                    break;
                }
            }

            refreshInfoMenu(saveFile);
        }

        if (m_keysDown & KEY_L)
        {
            while (true) {
                EditorConfig.SelectedPlayer--;
                if (EditorConfig.SelectedPlayer < 0)
                    EditorConfig.SelectedPlayer = 3;

                if (saveFile->players[EditorConfig.SelectedPlayer].Exists(saveFile)) {
                    break;
                }
            }

            refreshInfoMenu(saveFile);
        }

        for (u32 i = 0; i < 2; i++)
        {
            if ((m_keysDown & g_key[i]) && g_disabled[i])
            {
                // Check for input in the info menu
                if (playerNameBox.CheckActivate(g_CheckX[i], g_CheckY[i])) { // Player Name Box
                    // Find all references to the Player's id/name
                    std::vector<u32> m_PlayerIdReferences = findPlayerReferences(saveFile, &saveFile->players[EditorConfig.SelectedPlayer]);
                    saveFile->players[EditorConfig.SelectedPlayer].Name = u8tou16(playerNameBox.Activate().c_str());

                    // Replace all references to Player's id/name
                    for (u32 offset : m_PlayerIdReferences) {
                        saveFile->Write(offset, saveFile->players[EditorConfig.SelectedPlayer].PlayerId);
                        saveFile->Write(offset + 2, saveFile->players[EditorConfig.SelectedPlayer].Name, 8);
                    }

                    saveFile->SetChangesMade(true);
                }
                else if (playerWalletBox.CheckActivate(g_CheckX[i], g_CheckY[i])) {
                    saveFile->players[EditorConfig.SelectedPlayer].Wallet.value = static_cast<u32>(std::stoi(playerWalletBox.Activate()));
                    saveFile->SetChangesMade(true);
                }
                else if (playerSavingsBox.CheckActivate(g_CheckX[i], g_CheckY[i])) {
                    saveFile->players[EditorConfig.SelectedPlayer].Savings.value = static_cast<u32>(std::stoi(playerSavingsBox.Activate()));
                    saveFile->SetChangesMade(true);
                }
                else if (playerMedalsBox.CheckActivate(g_CheckX[i], g_CheckY[i])) {
                    saveFile->players[EditorConfig.SelectedPlayer].IslandMedals.value = static_cast<u32>(std::stoi(playerMedalsBox.Activate()));
                    saveFile->SetChangesMade(true);
                }
                else if (playerCouponsBox.CheckActivate(g_CheckX[i], g_CheckY[i])) {
                    saveFile->players[EditorConfig.SelectedPlayer].MeowCoupons.value = static_cast<u32>(std::stoi(playerCouponsBox.Activate()));
                    saveFile->SetChangesMade(true);
                }
            }
        }
    }

    EditorConfig.DrawingSubmenu = false;
}