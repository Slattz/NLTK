#include <3ds.h>
#include <string>
#include <map>
#include "gfx.h"
#include "nfs.h"
#include "save.h"
#include "utils.h"
#include "config.hpp"
#include "menus.h"
#include "editor/editor.h"
#include "InputManager.h"

extern u64 g_tid;
extern Config *config;
extern FS_MediaType currentMediaType;
extern std::map<u16, std::string> g_villagerDatabase;

u8* g_ItemBin;
u64 currentTitleId;
bool m_editorInitiated = false;

void Editor::Cleanup(void) {
    if (g_ItemBin) {
        delete[] g_ItemBin;
        g_ItemBin = NULL;
    }
    m_editorInitiated = false;
}

void Editor::Init(void) {
    // Load Item Database
    loadItemDatabase();

    // Load Villager Database
    LoadVillagerDatabase();

    Editor::Player::InitInfoGFX();

    g_ItemBin = new u8[0x2B720];
    file_read(g_ItemBin, "romfs:/Item.bin", 0x2B720);

    m_editorInitiated = true;
}

int Editor::Main(void) {
    if (!m_editorInitiated) {
        Editor::Init();
    }

GameSelect:
    Handle saveHandle;
    FS_Archive saveArch;

    {
        InputManager::Instance()->RefreshInput();
        if (config->Auto_loadprefGame && !InputManager::Instance()->IsButtonDown(KEY_DOWN) && is_ACNL(config->prefGame|0x4000000000000)) {
            g_tid = config->prefGame|0x4000000000000; // TODO: Support media type in config
        }
        else {
            g_tid = spawn_game_select_menu(&currentMediaType);
        }

        if (g_tid == 0) {
            Editor::Cleanup();
            return 0;
        }
    }

    bool successfullyOpenedArchive = openSaveArchive(&saveArch, g_tid, currentMediaType);
    if (!successfullyOpenedArchive && !(tryOpenSaveArchive(&saveArch, g_tid, &currentMediaType))) {
        MsgDisp(top, "Unable to Open the Save Archive\nSave file may not have been created!");
        Editor::Cleanup();
        return 0;
    }

    // Set current title id
    currentTitleId = g_tid;

    // Initialize a new Save class
    Save* saveFile = Save::Initialize(saveArch, &saveHandle, true);

    if (saveFile->GetSaveSize() != SIZE_SAVE) {
        MsgDisp(top, "Save file is the incorrect size!");
        Editor::Cleanup();
        saveFile->Close();
        return 0;
    }

    if (config->Auto_SaveBackup) {
        saveBackup(g_tid);
    }

    saveFile->FixSaveRegion();		//Update Region of the Save
    saveFile->FixInvalidBuildings(); //Fix any invalid buildings in save

    int mode = Editor::Spawn_MainMenu();

    saveFile->Close();

    if (mode == MODE_GAMESELECT) //Game Select
        goto GameSelect;

    Editor::Cleanup();
    return mode; //mode is always 0 when exiting the editor
}

void Editor::Draw() {
    while (aptMainLoop()) {
        InputManager::Instance()->RefreshInput();

        if (InputManager::Instance()->IsButtonActive(KEY_START)) {
            return; // Immediately exit.
        }

        if (InputManager::Instance()->IsButtonActive(KEY_B)) {
            if (Editor::PreviousDrawFunction() == nullptr) {
                // We've reached the end of the backchain. Exit the program immediately.
                return;
            }
        }

        draw_base_interface();

        if (Editor::DrawFunction != nullptr) {
            Editor::DrawFunction();
        }

        InputManager::Instance()->DrawCursor();
    }
}

void (*Editor::PreviousDrawFunction(void))(void) {
    if (Editor::DrawFunctionList.size() == 0) return nullptr;

    Editor::DrawFunction = Editor::DrawFunctionList.back();
    Editor::DrawFunctionList.pop_back();
    return Editor::DrawFunction;
}

void Editor::SetDrawFunction(void (*drawFunction)()) {
    Editor::DrawFunctionList.push_back(Editor::DrawFunction);
    Editor::DrawFunction = drawFunction;
}