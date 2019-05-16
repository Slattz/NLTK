#include <3ds.h>
#include <string>
#include <citro2d.h>
#include <algorithm>
#include "CTRFont.hpp"
#include "gfx.h"
#include "save.h"
#include "e_utils.h"
#include "utils.h"
#include "InputManager.h"
#include "imagebutton.h"
#include "menus.h"
#include "core/gui/ConfigMenu.hpp"
#include "core/gui/AboutMenu.hpp"
#include "gui/EditorMenu.hpp"
#include "gui/PlayerMenu.hpp"
#include "gui/AcreMenu.hpp"
#include "gui/ShopsMenu.hpp"
#include "gui/VillagerMenu.hpp"

ImageButton *TownButton = nullptr;
ImageButton *AcreButton = nullptr;
ImageButton *PlayerButton = nullptr;
ImageButton *VillagerButton = nullptr;
ImageButton *PWPButton = nullptr;
ImageButton *IslandButton = nullptr;
ImageButton *MainStreetButton = nullptr;
ImageButton *AboutButton = nullptr;

ImageButton *SaveButton = nullptr;
ImageButton *OptionsButton = nullptr;
ImageButton *GameSelectButton = nullptr;
ImageButton *TownManagerButton = nullptr;

static constexpr float TextSize = 0.6f;

static std::vector<Text *> ButtonText;
static std::vector<Text *> ColumnText;
static int ButtonIcon[] = {TOWN_ICON, ACRES_ICON, PROFILE_ICON, VILLAGERS_ICON,
                               PWP_ICON, ISLAND_ICON, SHOPS_ICON, ABOUT_ICON};

bool Editor::MainMenu::SaveCheck(void) {
    return MsgDisp(top, "Would you like to save changes\nmade to your save?", MsgTypeConfirm);
}

void Editor::MainMenu::Initialize(void) {
    TownButton = new ImageButton(15.f, 110.f, 70.f, 55.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    AcreButton = new ImageButton(89.f, 110.f, 70.f, 55.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    PlayerButton = new ImageButton(163.f, 110.f, 70.f, 55.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    VillagerButton = new ImageButton(237.f, 110.f, 70.f, 55.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    PWPButton = new ImageButton(15.f, 180.f, 70.f, 55.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    IslandButton = new ImageButton(89.f, 180.f, 70.f, 55.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    MainStreetButton = new ImageButton(163.f, 180.f, 70.f, 55.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    AboutButton = new ImageButton(237.f, 180.f, 70.f, 55.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);

    SaveButton = new ImageButton(220.f, 10.f, 80.f, 33.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    OptionsButton = new ImageButton(220.f, 60.f, 80.f, 33.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    GameSelectButton = new ImageButton(20.f, 10.f, 80.f, 33.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    TownManagerButton = new ImageButton(20.f, 60.f, 80.f, 33.f, 0, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);

    SaveButton->SetScale(1.15f, 0.6f);
    OptionsButton->SetScale(1.15f, 0.6f);
    GameSelectButton->SetScale(1.15f, 0.6f);
    TownManagerButton->SetScale(1.15f, 0.6f);

    ButtonText.push_back(new Text(COLOR_GREY, "Town", TextSize, TextSize, 37.f, 147.f));
    ButtonText.push_back(new Text(COLOR_GREY, "Acres", TextSize, TextSize, 110.f, 147.f));
    ButtonText.push_back(new Text(COLOR_GREY, "Players", TextSize, TextSize, 178.f, 147.f));
    ButtonText.push_back(new Text(COLOR_GREY, "Villagers", TextSize, TextSize, 248.f, 147.f));
    ButtonText.push_back(new Text(COLOR_GREY, "PWPs", TextSize, TextSize, 37.f, 217.f));
    ButtonText.push_back(new Text(COLOR_GREY, "Island", TextSize, TextSize, 110.f, 217.f));
    ButtonText.push_back(new Text(COLOR_GREY, "Shops", TextSize, TextSize, 182.f, 217.f));
    ButtonText.push_back(new Text(COLOR_GREY, "About", TextSize, TextSize, 256.f, 217.f));

    ColumnText.push_back(new Text(COLOR_GREY, "Game\nSelect", TextSize, TextSize, 44.f, 12.f));
    ColumnText.push_back(new Text(COLOR_GREY, "  Town\nManager", TextSize, TextSize, 40.f, 60.f));
    ColumnText.push_back(new Text(COLOR_GREY, "Save", TextSize, TextSize, 248.f, 18.f));
    ColumnText.push_back(new Text(COLOR_GREY, "Options", TextSize, TextSize, 240.f, 67.f));
}

void Editor::MainMenu::Cleanup(void) {
    delete TownButton;
    delete AcreButton;
    delete PlayerButton;
    delete VillagerButton;
    delete PWPButton;
    delete IslandButton;
    delete MainStreetButton;
    delete AboutButton;
    delete SaveButton;
    delete OptionsButton;
    delete GameSelectButton;
    delete TownManagerButton;
    TownButton = nullptr;
    AcreButton = nullptr;
    PlayerButton = nullptr;
    VillagerButton = nullptr;
    PWPButton = nullptr;
    IslandButton = nullptr;
    MainStreetButton = nullptr;
    AboutButton = nullptr;
    SaveButton = nullptr;
    OptionsButton = nullptr;
    GameSelectButton = nullptr;
    TownManagerButton = nullptr;

    for (const auto button : ButtonText) {
        delete button;
    }

    for (const auto column : ColumnText) {
        delete column;
    }

    ButtonText.clear();
    ColumnText.clear();
}

void Editor::MainMenu::Draw(void) {
    draw_base_interface();
    C2D_SceneBegin(bottom);
    DrawSprite(Common_ss, NLTK_ICON, 126, 10); //NLTK's Icon

    // Draw main buttons
    SaveButton->Draw();
    OptionsButton->Draw();
    GameSelectButton->Draw();
    TownManagerButton->Draw();

    for (int i = 0; i < 2; i++)
    {
        ColumnText[i]->Draw(); //Column 1 Text
        ColumnText[2+i]->Draw(); //Column 2 Text
    }

    // Draw sub buttons
    TownButton->Draw();
    AcreButton->Draw();
    PlayerButton->Draw();
    VillagerButton->Draw();
    PWPButton->Draw();
    IslandButton->Draw();
    MainStreetButton->Draw();
    AboutButton->Draw();

    for (int i = 0; i < 4; i++)
    {
        DrawSprite(Editor_ss, ButtonIcon[i], 39+(74*i), 114, GreenFilter); //Row 1 Icons
        DrawSprite(Editor_ss, ButtonIcon[4+i], 39+(74*i), 184, GreenFilter); //Row 2 Icons
        ButtonText[i]->Draw(); //Row 1 Text
        ButtonText[i+4]->Draw(); //Row 2 Text
    }

    C2D_SceneBegin(top);
    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

ReturnMode Editor::Spawn_MainMenu()
{
    Editor::MainMenu::Initialize();

    while (aptMainLoop())
    {
        checkIfCardInserted();

        Editor::MainMenu::Draw();
        InputManager::Instance()->RefreshInput();

        if (InputManager::Instance()->IsButtonDown(KEY_START))
        {
            return ReturnMode::Exit;
        }

        else if (InputManager::Instance()->IsButtonDown(KEY_B))
        {
            return ReturnMode::MainMenu;
        }

        if (TownButton->IsActive()) //Town Menu
            MsgDisp(top, "Town Menu Coming Soon!");

        else if (AcreButton->IsActive()) //Acres Menu
            Editor::Spawn_AcresMenu();

        else if (PlayerButton->IsActive()) //Player Menu
            Editor::Spawn_PlayerMenu();

        else if (VillagerButton->IsActive()) { //Villagers Menu
            VillagerEditor::Spawn();
        }

        else if (PWPButton->IsActive()) //Pwp Menu
            MsgDisp(top, "PWP Menu Coming Soon!");

        else if (IslandButton->IsActive()) //Island Menu
            MsgDisp(top, "Island Menu Coming Soon!");

        else if (MainStreetButton->IsActive()) //Main Street Menu
            MsgDisp(top, "Shops Menu Coming Soon!");

        else if (AboutButton->IsActive()) //About Menu
            Core::Spawn_AboutMenu();

        /* Menu Columns */
        else if (OptionsButton->IsActive()) //Options Column
            Core::Spawn_ConfigMenu();

        else if (GameSelectButton->IsActive()) //Game Select Column
            return ReturnMode::GameSelect;

        else if (TownManagerButton->IsActive()) //Town Manager Column
            MsgDisp(top, "Town Manager is currently disabled.");
            //return ReturnMode::Manager;

        else if (SaveButton->IsActive()) //Save Column
        {
            if (Editor::MainMenu::SaveCheck())
            {
                if (Save::Instance()->Commit(false))
                    MsgDisp(top, "Save Succeeded!");
                else
                    MsgDisp(top, "Save Failed!");
            }
        }
    }
    Editor::MainMenu::Cleanup();
    return ReturnMode::Exit;
}
