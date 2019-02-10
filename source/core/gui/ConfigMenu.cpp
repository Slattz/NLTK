#include <3ds.h>
#include <string>
#include <vector>
#include <citro2d.h>
#include "CTRFont.hpp"
#include "InputManager.h"
#include "core/gui/ConfigMenu.hpp"
#include "imagebutton.h"
#include "label.h"
#include "menus.h"
#include "config.hpp"
#include "utils.h"

#define CONFIG_BOOL_AMOUNT 4
#define CONFIG_STRING_AMOUNT 0
#define CONFIG_INT_AMOUNT 3

static std::vector<ImageButton *> ConfigCheckboxes;
static std::vector<Label *> ConfigCheckboxText;
static const char *ConfigCheckboxStrings[CONFIG_BOOL_AMOUNT] = {"Auto Update", "Auto Save Backup", "Auto Load Preferred Game", "Debug"};

void Core::ConfigMenu::Initialize(void) {
    bool *ConfigBools[CONFIG_BOOL_AMOUNT] = {&Config::Instance()->Auto_Update, &Config::Instance()->Auto_SaveBackup, 
        &Config::Instance()->Auto_loadprefGame, &Config::Instance()->IsDebug};

    for (u32 i = 0; i < CONFIG_BOOL_AMOUNT; i++) {
        Label* Lbl = new Label({52, 23+(28*i)}, {0, 0}, COLOR_TRANSPARENT, COLOR_WHITE, std::string(ConfigCheckboxStrings[i]));
        Lbl->SetTextSize(0.6f, 0.6f);
        ConfigCheckboxText.push_back(Lbl);

        ImageButton* Checkbox = new ImageButton({20, 20+(28*i)}, {24, 24}, COLOR_TRANSPARENT, KEY_A | KEY_TOUCH, 
        (*ConfigBools[i] == true ? CHECKBOX_FILLED : CHECKBOX_EMPTY), Common_ss);

        ConfigCheckboxes.push_back(Checkbox);
    }
}

void Core::ConfigMenu::Cleanup(void) {
    for (const auto button : ConfigCheckboxes) {
        delete button;
    }

    for (const auto text : ConfigCheckboxText) {
        delete text;
    }

    ConfigCheckboxes.clear();
    ConfigCheckboxText.clear();
}

void Core::Draw_ConfigMenu(void)
{
    draw_base_interface();
    C2D_SceneBegin(bottom);

    for (const auto checkbox : ConfigCheckboxes) {
        checkbox->Draw();
    }

    for (const auto text : ConfigCheckboxText) {
        text->Draw();
    }
    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}

void Core::Spawn_ConfigMenu(void) {
    bool *ConfigBools[CONFIG_BOOL_AMOUNT] = {&Config::Instance()->Auto_Update, &Config::Instance()->Auto_SaveBackup, 
        &Config::Instance()->Auto_loadprefGame, &Config::Instance()->IsDebug};

    // Cleanup any unfreed resources.
    Core::ConfigMenu::Cleanup();

    // Initialize resources.
    Core::ConfigMenu::Initialize();

    while (aptMainLoop())
    {
        checkIfCardInserted();

        Draw_ConfigMenu();
        InputManager::Instance()->RefreshInput();

        if (InputManager::Instance()->IsButtonDown(KEY_B))
            break;

        for (u32 i = 0; i < CONFIG_BOOL_AMOUNT; i++) {
            if (ConfigCheckboxes[i]->IsDown()) {
                *ConfigBools[i] = !(*ConfigBools[i]);
                ConfigCheckboxes[i]->SetImageId(*ConfigBools[i] == true ? CHECKBOX_FILLED : CHECKBOX_EMPTY);
            }
        }
    }
    Config::Instance()->Save();
    Core::ConfigMenu::Cleanup();
}