#pragma once

#ifndef EGFX_H
#define EGFX_H

#include "common.h"
#include "core/textbox.h"

extern C2D_ImageTint* AcreTint;

void InitAcreGFX(Save *saveFile, const u8 LoopMax, const u8 GridXMax, const u8 GridXStartPos, const u8 GridYStartPos, const u8 ByteSkip, u32 Offset);
//void draw_player_menu_appearance(u8* savebuf, int selectedplayer);
//void draw_player_menu_house(u8* savebuf, int selectedplayer);
//void draw_player_menu_mailbox(u8* savebuf, int selectedplayer);
void draw_save_acres(Save *saveFile);
void draw_acre_selection_panel(void);

extern TextBox playerNameBox;
extern TextBox playerWalletBox;
extern TextBox playerSavingsBox;
extern TextBox playerMedalsBox;
extern TextBox playerCouponsBox;
extern std::vector<Control*> editorPlayerInfoControls;
extern std::vector<Control*> acreEditorControls;
extern s32 acreEditorSelectedAcre;

#endif