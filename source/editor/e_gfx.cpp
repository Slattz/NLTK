#include "common.h"
#include "core/label.h"
#include "core/imagebutton.h"
#include "editor/e_gfx.h"

C2D_ImageTint* AcreTint = new C2D_ImageTint[sizeof(C2D_ImageTint)];

std::vector<Control*> editorPlayerInfoControls = {};
std::vector<Control*> acreEditorControls = {};

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

s32 acreEditorSelectedAcre = -1;

void InitEditorGFX(void)
{
    C2D_PlainImageTint(AcreTint, COLOR_DARK_GREY, 0.5f);

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

	// Create Acre Editor Controls
}

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

void draw_editor_main_menu(void)
{
    static bool TextInit = false;
    static const float TextSize = 0.6f;
    static std::vector<Text> ButtonText;
    static std::vector<Text> ColumnText;
    static int ButtonIcon[] = {TOWN_ICON, ACRES_ICON, PROFILE_ICON, VILLAGERS_ICON,
                          PWP_ICON, ISLAND_ICON, MAIN_STREET_ICON, ABOUT_ICON};

    if (!TextInit) {
        ButtonText.push_back(Text(COLOR_GREY, "Town", TextSize, TextSize, 37.f, 147.f));
        ButtonText.push_back(Text(COLOR_GREY, "Acres", TextSize, TextSize, 110.f, 147.f));
        ButtonText.push_back(Text(COLOR_GREY, "Players", TextSize, TextSize, 178.f, 147.f));
        ButtonText.push_back(Text(COLOR_GREY, "Villagers", TextSize, TextSize, 248.f, 147.f));
        ButtonText.push_back(Text(COLOR_GREY, "PWPs", TextSize, TextSize, 37.f, 217.f));
        ButtonText.push_back(Text(COLOR_GREY, "Island", TextSize, TextSize, 110.f, 217.f));
        ButtonText.push_back(Text(COLOR_GREY, "Main Street", 0.55f, 0.55f, 168.f, 217.f));
        ButtonText.push_back(Text(COLOR_GREY, "About", TextSize, TextSize, 256.f, 217.f));

        ColumnText.push_back(Text(COLOR_GREY, "Game\nSelect", TextSize, TextSize, 44.f, 12.f));
        ColumnText.push_back(Text(COLOR_GREY, "  Town\nManager", TextSize, TextSize, 40.f, 60.f));
        ColumnText.push_back(Text(COLOR_GREY, "Save", TextSize, TextSize, 248.f, 18.f));
        ColumnText.push_back(Text(COLOR_GREY, "Options", TextSize, TextSize, 240.f, 67.f));
        TextInit = true;
    }

    draw_base_interface();
    C2D_SceneBegin(bottom);
    DrawSprite(Common_ss, NLTK_ICON, 126, 10); //NLTK's Icon

    for (int i = 0; i < 2; i++)
    {
        DrawSprite(Editor_ss, BUTTON_MAIN, 20,  10+(50*i), nullptr, 1.15, 0.6);  //w = 80, h = 33
        DrawSprite(Editor_ss, BUTTON_MAIN, 220, 10+(50*i), nullptr, 1.15, 0.6);  //w = 80, h = 33
        ColumnText[i].Draw(); //Column 1 Text
        ColumnText[2+i].Draw(); //Column 2 Text
    }

    for (int i = 0; i < 4; i++)
    {
        DrawSprite(Editor_ss, BUTTON_MAIN, 15+(74*i), 110); //Row 1 Buttons
        DrawSprite(Editor_ss, BUTTON_MAIN, 15+(74*i), 180); //Row 2 Buttons
        DrawSprite(Editor_ss, ButtonIcon[i], 39+(74*i), 114, GreenFilter); //Row 1 Icons
        DrawSprite(Editor_ss, ButtonIcon[4+i], 39+(74*i), 184, GreenFilter); //Row 2 Icons
        ButtonText[i].Draw(); //Row 1 Text
        ButtonText[i+4].Draw(); //Row 2 Text
    }

    C2D_SceneBegin(top);
    draw_centered_text(0, 400, 80, 0, 1.1, 1.1, COLOR_GREY, "Editor Main Menu!");
    draw_cursor();
    C3D_FrameEnd(0);
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

	draw_cursor();
    C3D_FrameEnd(0);
}

void draw_player_menu_top(Save *saveFile, int selectedplayer, u32 LColor, u32 RColor) {
    static bool Init = false;
    static Text LButton;
    static Text RButton;
    static std::vector<Text> PlayerNameText;
    static std::vector<Text> PlayerNumText;
	u32 Ptext_colour;

    if (!Init)
    {
        LButton = Text(COLOR_GREY, FONT_NDS_L, 1.3f, 1.3f, 5.f, 210.f);
        RButton = Text(COLOR_GREY, FONT_NDS_R, 1.3f, 1.3f, 375.f, 210.f);
        for (int i = 0; i < 4; i++) {
            if (saveFile->players[i].Exists(saveFile)) {
                PlayerNameText.push_back(Text(COLOR_GREY, u16tou8(saveFile->players[i].Name).c_str(), 0.5f, 0.5f));
                PlayerNameText[i].CenterInBounds(18.f + (i * 100.f), 150.f, 64.f, 15.f);

                PlayerNumText.push_back(Text(COLOR_GREY, Format("Player %d", i + 1).c_str(), 0.5f, 0.5f));
                PlayerNumText[i].CenterInBounds(18.f + (i * 100.f), 25.f, 64.f, 20.f);
            }
        }
        Init = true;
    }

	draw_base_interface();

	for (int i = 0; i < 4; i++) {
		if (saveFile->players[i].Exists(saveFile)) {
			C2D_DrawImageAt(saveFile->players[i].m_TPCPic, (float)(100 * i) + 18.f, 45.f, 0.5f, nullptr, 1.f, 1.f);  //WxH: 64x104

			if (i == selectedplayer)
				Ptext_colour = COLOR_WHITE;
			else
				Ptext_colour = COLOR_GREY;

			PlayerNameText[i].SetColor(Ptext_colour).Draw();
            PlayerNumText[i].SetColor(Ptext_colour).Draw();

		}
	}
    LButton.SetColor(LColor).Draw();
    RButton.SetColor(RColor).Draw();
}

void draw_player_menu(Save *saveFile, int selectedplayer, int selectedmode, u32 LColor, u32 RColor)
{
    u32 ButtonIcon[] = {PLYR_ABOUT, PLYR_INVENTORY, PLYR_APPEARANCE, PLYR_HOUSE,
                        PLYR_PATTERN, PLYR_MAILBOX, PLYR_MAILBOX_UNREAD};
    const char* ButtonText[] = {"Info", "Inventory", "Appearance",
                            "House", "Patterns", "Mail"};

	draw_player_menu_top(saveFile, selectedplayer, LColor, RColor);
    C2D_SceneBegin(bottom);

	/* Sidebar Stuff */
    C2D_DrawRectSolid(0, 0, 0, 58, 240, C2D_Color32(39,163,49,255)); //Sidebar Background
    if (selectedmode != -1 && selectedmode < 6)
        C2D_DrawRectSolid(0, 4+(40*selectedmode), 0, 55, 40, C2D_Color32(44,183,53,255)); //

    C2D_DrawRectSolid(55, 0, 0, 3, 240, COLOR_MENU_BARS); //Vertical Right Bar
    C2D_DrawRectSolid(0, 0, 0, 55, 3, COLOR_MENU_BARS); //Horizontal Top Bar

    for (int i = 0; i < 6; i++)
    {
        /* Icons */
        if (i == 5 && CheckUnreadMail(selectedplayer))
            DrawSprite(Players_ss, ButtonIcon[i+1], 15, 4+(40*i)); //If Unread Mail

        else
            DrawSprite(Players_ss, ButtonIcon[i], 15, 4+(40*i));

        /* Text */
        if (i == 1)
            draw_centered_text(1, 53, 35+(39*i), 8, 0.38, 0.38, COLOR_GREY, ButtonText[i]); //Appearance Text

        else if (i == 2)
            draw_centered_text(1, 53, 35+(39*i), 8, 0.35, 0.38, COLOR_GREY, ButtonText[i]); //Inventory Text

        else
            draw_centered_text(1, 53, 35+(39*i), 8, 0.4, 0.4, COLOR_GREY, ButtonText[i]); //Text
    }

    draw_cursor();
    C3D_FrameEnd(0);
}

void draw_player_menu_info(Save *saveFile, int selectedPlayer, u32 LColor, u32 RColor) {
	draw_player_menu_top(saveFile, selectedPlayer, LColor, RColor);
	C2D_SceneBegin(bottom);

	// Draw Controls
	for (Control *c : editorPlayerInfoControls) {
		c->Draw();
	}

	draw_cursor();
	C3D_FrameEnd(0);
}

void draw_player_menu_inventory(Save *saveFile, int selectedplayer, u32 LColor, u32 RColor)
{
    int x = 11;
    int y = 50;

	draw_player_menu_top(saveFile, selectedplayer, LColor, RColor);
    C2D_SceneBegin(bottom);

    for (int i = 0; i < 16; ++i)
    {
        if (i % 8 == 0 && i != 0)
        {
            y += 38;
            x = 11;
        }

        Item item = saveFile->players[selectedplayer].Pockets[i];
        DrawSprite(Common_ss, ITEM_HOLE, x-16, y-16);
        //DrawSprite(Items_ss, GetIndex(item.Icon), x, y);
        DrawSprite(Items_ss, 0, x, y);

        x += 38;
    }

    draw_cursor();
    C3D_FrameEnd(0);
}
/*
void draw_player_menu_appearance(u8* savebuf, int selectedplayer)
{
    return;
}

void draw_player_menu_house(u8* savebuf, int selectedplayer)
{
    return;
}

void draw_player_menu_patterns(u8* savebuf, int selectedplayer)
{
    return;
}

void draw_player_menu_mailbox(u8* savebuf, int selectedplayer)
{
    return;
}*/

