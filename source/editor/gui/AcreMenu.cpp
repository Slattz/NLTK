#include <3ds.h>
#include <string>
#include <algorithm>
#include <citro2d.h>
#include "CTRFont.hpp"
#include "unicode.h"
#include "item.h"
#include "imagebutton.h"
#include "gfx.h"
#include "save.h"
#include "e_utils.h"
#include "utils.h"
#include "InputManager.h"
#include "menus.h"
#include "gui/AcreMenu.hpp"

#define ACRE_ID_MAX 218

AcreSettings AcreConfig;
C2D_ImageTint* AcreTint = new C2D_ImageTint[sizeof(C2D_ImageTint)];
std::vector<Control*> acreEditorControls = {};

void onAcreClick(Button *sender);

void Editor::Acre::InitAcreGFX(const u8 LoopMax, const u8 GridXMax,
    const u8 GridXStartPos, const u8 GridYStartPos, const u8 ByteSkip, u32 Offset) {
    /*
    LoopMax: How many times to loop
    GridXMax: How many acres per row
    GridXStartPos: Start X Position for drawing
    GridYStartPos: Start Y Position for drawing
    ByteSkip: Num of bytes to skip when moving to new row
    Offset: offset of acre IDs in save
    */

    C2D_PlainImageTint(AcreTint, COLOR_DARK_GREY, 0.5f);
    acreEditorControls.clear();
    float scale = 1; // The scale of the acre editor image buttons

    u8  acre = 0;           //acre ID
    u32 GridX = 0;          //X position of acre
    u32 GridY = 0;          //Y position of acre
    //u32 GridXOffset = (320 - (40 * scale) * GridXMax) / 2; // Offset to center the controls horizontally

    for (u32 i = 0; i < LoopMax; i++)
    {
        GridX = i % GridXMax; //Get remainder for the row
        if (GridX == 0 && i != 0)
        {
            GridY += 40 * scale; // 40 == height of acre img
            Offset += ByteSkip;
        }

        acre = Save::Instance()->ReadU8(Offset + i * 2);
        ImageButton *acreButton = new ImageButton(GridXStartPos + (40 * scale * GridX), GridYStartPos + GridY, 40 * scale, 40 * scale,
            COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, ACRE_PNG + acre, Acres_ss);
        acreButton->SetScale(scale);
        acreButton->SetImageTint(COLOR_YELLOW);

        acreButton->SetCallback(onAcreClick);
        acreEditorControls.push_back(acreButton);
    }
}

static void Draw_SelectionPanel(void) {
    int numAcresShown = 5; // 5 acres shown at a time max
    u32 xLocation = 130;
    u32 yLocation = 120;
    s32 startAcreId = AcreConfig.SelectedAcre - 2;

    for (int i = 0; i < numAcresShown; i++) {
        s32 acreId = startAcreId + i;
        if (acreId < 0 || acreId > ACRE_ID_MAX) {
            continue;
        }

        if (i == 2) {
            DrawSprite(Acres_ss, ACRE_PNG + acreId, xLocation, yLocation - 20);
            xLocation += 45;
        }
        else {
            DrawSprite(Acres_ss, ACRE_PNG + acreId, xLocation, yLocation - 10, AcreTint, 0.5, 0.5);
            xLocation += 25;
        }
    }
}

static void Draw_AcresMenu()
{

    draw_base_interface();

    // Draw selected acre (if one is selected)
    /*if (currentAcreId > -1 && currentAcreId <= ACRE_ID_MAX) {
        DrawSprite(Acres_ss, ACRE_PNG + currentAcreId, (400 / 2) - (40 / 2), (240 / 2) - (40 / 2));
    }*/

    if (AcreConfig.SelectedAcre > -1 && AcreConfig.SelectedAcre <= ACRE_ID_MAX)
    {
        Draw_SelectionPanel();
    }

    C2D_SceneBegin(bottom);

    for (Control *c : acreEditorControls) {
        c->Draw();
    }

    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

s32 currentAcreId = -1;
ImageButton *currentAcreButton;

void view_acres_town_full()
{
    Editor::Acre::InitAcreGFX(42, 7, 20, 0, 0, MAP_ACRES); // Ends at x = 230px (resized to 30x30 images)
}

void view_acres_island_full()
{
    Editor::Acre::InitAcreGFX(16, 4, 80, 40, 0, ISLAND_ACRES);
}

void view_acres_town_map()
{
    Editor::Acre::InitAcreGFX(20, 5, 60, 40, 4, MAP_ACRES + 0x10);
}

void view_acres_island_map()
{
    Editor::Acre::InitAcreGFX(4, 2, 120, 80, 4, ISLAND_ACRES + 0xA);
}

// Acre Menu
void Editor::Spawn_AcresMenu()
{
    if (AcreConfig.DrawingMenu)
        return;

    AcreConfig.DrawingMenu = true;
    AcreConfig.SelectedAcre = -1;
    currentAcreId = -1;
    currentAcreButton = nullptr;

    // Init Acre Graphics
    view_acres_town_full();

    while (aptMainLoop())
    {
        checkIfCardInserted();

        Draw_AcresMenu();
        InputManager::Instance()->RefreshInput();

        if (InputManager::Instance()->IsButtonActive(KEY_B)) {
            break;
        }

        if (AcreConfig.SelectedAcre > 0 && InputManager::Instance()->IsButtonDown(KEY_DLEFT)) {
            AcreConfig.SelectedAcre--;
        }
        else if (AcreConfig.SelectedAcre > -1 && AcreConfig.SelectedAcre < ACRE_ID_MAX && InputManager::Instance()->IsButtonDown(KEY_DRIGHT)) {
            AcreConfig.SelectedAcre++;
        }

        if (AcreConfig.SelectedAcre > -1 && AcreConfig.SelectedAcre <= ACRE_ID_MAX && InputManager::Instance()->IsButtonDown(KEY_A)) {
            currentAcreButton->SetImageId(ACRE_PNG + AcreConfig.SelectedAcre);
            u32 writeOffset = MAP_ACRES
                + std::distance(acreEditorControls.begin(), std::find(acreEditorControls.begin(), acreEditorControls.end(), currentAcreButton)) * 2;
            Save::Instance()->Write(writeOffset, static_cast<u8>(AcreConfig.SelectedAcre));

            // Clear selected acre
            AcreConfig.SelectedAcre = -1;
            currentAcreButton->SetActive(false);
            currentAcreButton = nullptr;
            currentAcreId = -1;
        }

        for (Control *c : acreEditorControls) {
            auto imageButton = reinterpret_cast<ImageButton *>(c);
            if (imageButton && imageButton->IsActive()) { // cast was successful?
                imageButton->Activate();
                break;
            }
        }
    }

    AcreConfig.DrawingMenu = false;
}

/* Callbacks */

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
        AcreConfig.SelectedAcre = currentAcreId;
    }
}