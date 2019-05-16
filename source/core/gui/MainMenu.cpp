#include <3ds.h>
#include <string>
#include <vector>
#include <citro2d.h>
#include "CTRFont.hpp"
#include "InputManager.h"
#include "core/gui/MainMenu.hpp"
#include "config.hpp"
#include "label.h"
#include "imagebutton.h"
#include "manager/manager.h"
#include "editor/editor.h"
#include "core/gui/AboutMenu.hpp"
#include "core/gui/ConfigMenu.hpp"

ImageButton* EditorIcon = nullptr;
ImageButton* ManagerIcon = nullptr;
ImageButton* AboutBtn = nullptr;
ImageButton* OptionsBtn = nullptr;
Label* ManagerTempLbl = nullptr; //Temporary while Manager can only be enabled in Debug
static std::vector<Label *> MenuText;

void Core::MainMenu::Initialize(void) {
    Label* Editor = new Label({100, 150}, {0, 0}, COLOR_TRANSPARENT, COLOR_WHITE, "Editor");
    Editor->SetTextSize(0.5f, 0.5f);
    MenuText.push_back(Editor);

    Label* Manager = new Label({180, 150}, {0, 0}, COLOR_TRANSPARENT, COLOR_WHITE, "Manager");
    Manager->SetTextSize(0.5f, 0.5f);
    MenuText.push_back(Manager);
    ManagerTempLbl = Manager;

    Label* About = new Label({42, 38}, {0, 0}, COLOR_TRANSPARENT, COLOR_WHITE, "About");
    About->SetTextSize(0.5f, 0.5f);
    MenuText.push_back(About);

    Label* Options = new Label({236, 38}, {0, 0}, COLOR_TRANSPARENT, COLOR_WHITE, "Options");
    Options->SetTextSize(0.5f, 0.5f);
    MenuText.push_back(Options);

    EditorIcon = new ImageButton(90, 90, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, EDITOR_ICON, Common_ss);
    ManagerIcon = new ImageButton(180, 90, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, MANAGER_ICON, Common_ss);
    AboutBtn = new ImageButton(20, 30, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    AboutBtn->SetScale(1.15f, 0.6f);
    OptionsBtn = new ImageButton(220, 30, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, BUTTON_MAIN, Editor_ss);
    OptionsBtn->SetScale(1.15f, 0.6f);
}

void Core::MainMenu::Cleanup(void) {
    for (auto labels : MenuText) {
        delete labels;
        labels = nullptr;
    }

    delete EditorIcon;
    delete ManagerIcon;
    delete AboutBtn;
    delete OptionsBtn;
}

void Core::Draw_MainMenu(void)
{
    draw_base_interface();
    C2D_SceneBegin(bottom);
    DrawSprite(Common_ss, NLTK_ICON, 126.f, 10.f); //NLTK's Icon
    AboutBtn->Draw();
    OptionsBtn->Draw();
    ManagerTempLbl->Visible = false;
    ManagerIcon->Visible = false;

    for (const auto labels : MenuText) {
        labels->Draw();
    }

    EditorIcon->Draw(); //Editor Icon
    //ManagerIcon->Draw(); //Manager Icon

    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

ReturnMode Core::Spawn_MainMenu(void) {
    static ReturnMode ret = ReturnMode::None;

    Core::MainMenu::Initialize();
    while (aptMainLoop()) {
        Draw_MainMenu();
        InputManager::Instance()->RefreshInput();

        if (InputManager::Instance()->IsButtonActive(KEY_START)) {
            return ReturnMode::Exit;
        }

        if (EditorIcon->IsActive()) //Editor Icon
            ret = Editor::Main();
/*
        else if (ManagerIcon->IsActive() && Config::Instance()->IsDebug) //Manager Icon
            ret = manager_main();
*/
        else if (AboutBtn->IsActive()) //About Menu
            Core::Spawn_AboutMenu();

        else if (OptionsBtn->IsActive()) //Config Menu
            Core::Spawn_ConfigMenu();

        if (ret == ReturnMode::Exit) {
            break;
        }

        else if (ret == ReturnMode::Editor) {
            ret = Editor::Main();
        }

        else if (ret == ReturnMode::Manager) {
            ret = manager_main();
        }
    }
    Core::MainMenu::Cleanup();
    return ReturnMode::Exit;
}