#include <3ds.h>
#include <string>
#include <citro2d.h>
#include <algorithm>
#include "gfx.h"
#include "save.h"
#include "e_utils.h"
#include "utils.h"
#include "cursor.h"
#include "editor/editor.h"
#include "core/textbox.h"
#include "core/imagebutton.h"
#include "common.h"
#include "menus.h"
#include "e_menus.h"

extern Cursor g_cursor;
extern s16  g_CheckX[2];
extern s16  g_CheckY[2];
extern bool g_disabled[2];
extern u32  g_key[2];

static bool drawingMenu = false;
static bool drawingSubmenu = false;
s32 currentAcreId = -1;
ImageButton *currentAcreButton;

bool check_if_saving() {
    return MsgDisp(top, "Would you like to save changes\nmade to your save?", MsgTypeConfirm);
}


void view_acres_town_full(Save *saveFile)
{
    InitAcreGFX(saveFile, 42, 7, 20, 0, 0, MAP_ACRES); // Ends at x = 230px (resized to 30x30 images)
}

void view_acres_island_full(Save *saveFile)
{
    InitAcreGFX(saveFile, 16, 4, 80, 40, 0, ISLAND_ACRES);
}

void view_acres_town_map(Save *saveFile)
{
    InitAcreGFX(saveFile, 20, 5, 60, 40, 4, MAP_ACRES + 0x10);
}

void view_acres_island_map(Save *saveFile)
{
    InitAcreGFX(saveFile, 4, 2, 120, 80, 4, ISLAND_ACRES + 0xA);
}

// Acre Menu
void spawn_acres_menu(Save *saveFile)
{
    if (drawingMenu)
        return;

    drawingMenu = true;
    acreEditorSelectedAcre = -1;
    currentAcreId = -1;
    currentAcreButton = nullptr;

    // Init Acre Graphics
    view_acres_town_full(saveFile);

    while (aptMainLoop())
    {
        checkIfCardInserted();

        hidScanInput();
        
        if (true) {
            draw_save_acres(saveFile);
        }

        updateCursorInfo();

        if (hidKeysDown() & KEY_B) {
            break;
        }

        if (acreEditorSelectedAcre > 0 && hidKeysDown() & KEY_DLEFT) {
            acreEditorSelectedAcre--;
        }
        else if (acreEditorSelectedAcre > -1 && acreEditorSelectedAcre < ACRE_ID_MAX && hidKeysDown() & KEY_DRIGHT) {
            acreEditorSelectedAcre++;
        }

        if (acreEditorSelectedAcre > -1 && acreEditorSelectedAcre <= ACRE_ID_MAX && hidKeysDown() & KEY_A) {
            currentAcreButton->SetImageId(ACRE_PNG + acreEditorSelectedAcre);
            u32 writeOffset = MAP_ACRES
                + std::distance(acreEditorControls.begin(), std::find(acreEditorControls.begin(), acreEditorControls.end(), currentAcreButton)) * 2;
            saveFile->Write(writeOffset, static_cast<u8>(acreEditorSelectedAcre));

            // Clear selected acre
            acreEditorSelectedAcre = -1;
            currentAcreButton->SetActive(false);
            currentAcreButton = nullptr;
            currentAcreId = -1;
        }

        for (int i = 0; i < 2; i++)
        {
            if (hidKeysDown() & g_key[i] && g_disabled[i])
            {
                for (Control *c : acreEditorControls) {
                    auto imageButton = reinterpret_cast<ImageButton*>(c);
                    if (imageButton && imageButton->CheckActivate(g_CheckX[i], g_CheckY[i])) { // cast was successful?
                        imageButton->Activate();
                        break;
                    }
                }
            }
        }
    }

    drawingMenu = false;
}

void onAcreClick(Button *sender) {
    if (sender != nullptr) {

        // Disable all buttons
        if (currentAcreButton != nullptr) {
            for (Control *c : acreEditorControls) {
                reinterpret_cast<ImageButton *>(c)->SetActive(false);
            }
        }

        sender->SetActive(true);
        currentAcreButton = reinterpret_cast<ImageButton *>(sender);
        currentAcreId = currentAcreButton->GetImageId() - ACRE_PNG;
        acreEditorSelectedAcre = currentAcreId;
    }
}