#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "unicode.h"
#include "gfx.h"
#include "save.h"
#include "item.h"
#include "utils.h"
#include "cursor.h"
#include "pattern.h"
#include "core/label.h"
#include "core/imagebutton.h"
#include "editor/e_utils.h"
#include "editor/e_menus.h"
#include "editor/e_gfx.h"

extern Cursor g_cursor;

C2D_ImageTint* AcreTint = new C2D_ImageTint[sizeof(C2D_ImageTint)];

std::vector<Control*> acreEditorControls = {};



s32 acreEditorSelectedAcre = -1;

void InitAcreGFX(Save *saveFile, const u8 LoopMax, const u8 GridXMax,
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

        acre = saveFile->ReadU8(Offset + i * 2);
        ImageButton *acreButton = new ImageButton(GridXStartPos + (40 * scale * GridX), GridYStartPos + GridY, 40 * scale, 40 * scale,
            COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, ACRE_PNG + acre, Acres_ss);
        acreButton->SetScale(scale);
        acreButton->SetImageTint(COLOR_YELLOW);

        acreButton->SetCallback(onAcreClick);
        acreEditorControls.push_back(acreButton);
    }
}

void draw_acre_selection_panel(void) {
    int numAcresShown = 5; // 5 acres shown at a time max
    u32 xLocation = 130;
    u32 yLocation = 120;
    s32 startAcreId = acreEditorSelectedAcre - 2;

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

void draw_save_acres(Save *saveFile)
{

    draw_base_interface();

    // Draw selected acre (if one is selected)
    /*if (currentAcreId > -1 && currentAcreId <= ACRE_ID_MAX) {
        DrawSprite(Acres_ss, ACRE_PNG + currentAcreId, (400 / 2) - (40 / 2), (240 / 2) - (40 / 2));
    }*/

    if (acreEditorSelectedAcre > -1 && acreEditorSelectedAcre <= ACRE_ID_MAX) {
        draw_acre_selection_panel();
    }

    C2D_SceneBegin(bottom);

    for (Control *c : acreEditorControls) {
        c->Draw();
    }

    g_cursor.Draw();
    C3D_FrameEnd(0);
}

