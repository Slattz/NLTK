#include <3ds.h>
#include <string>
#include "gfx.h"
#include "file.hpp"
#include "nfs.h"
#include "save.h"
#include "utils.h"
#include "config.hpp"
#include "editor/editor.h"
#include "InputManager.h"
#include "core/gui/GameSelectMenu.hpp"

extern FS_MediaType currentMediaType;

File* g_ItemBin;
u64 currentTitleId;
bool m_editorInitiated = false;

void Editor::Cleanup(void) {
    Editor::Player::CleanupInfoGFX();
    if (g_ItemBin) {
        g_ItemBin->Close();
        g_ItemBin = nullptr;
    }
    m_editorInitiated = false;
}

void Editor::Init(void) {
    // Load Item Database
    loadItemDatabase();

    // Load Villager Database
    LoadVillagerDatabase();

    Editor::Player::InitInfoGFX();
    g_ItemBin = new File();
    Result res = File::Open(*g_ItemBin, "romfs:/Item.bin", File::RB);
    if (R_FAILED(res)) {
        MsgDisp(top, Format("Failed opening Item.bin:\nResult: %X", res));
        if (g_ItemBin != nullptr) delete g_ItemBin;
        g_ItemBin = nullptr;
    }

    m_editorInitiated = true;
}

ReturnMode Editor::Main(void) {
    ReturnMode retMode = ReturnMode::None;
    if (!m_editorInitiated) {
        Editor::Init();
    }

    while (retMode == ReturnMode::GameSelect || retMode == ReturnMode::None) {
        FS_Archive saveArch;

        InputManager::Instance()->RefreshInput();
        if (Config::Instance()->Auto_loadprefGame && !InputManager::Instance()->IsButtonDown(KEY_DOWN) && is_ACNL(Config::Instance()->prefGame|0x4000000000000)) {
            currentTitleId = Config::Instance()->prefGame|0x4000000000000; // TODO: Support media type in config
            currentMediaType = MEDIATYPE_GAME_CARD;
        }
        else {
            currentTitleId = Core::Spawn_GameSelectMenu(currentMediaType);
        }

        if (currentTitleId == 0) {
            Editor::Cleanup();
            return ReturnMode::Exit;
        }

        Result res = FS::OpenSaveArchive(&saveArch, currentMediaType, currentTitleId);
        if (R_FAILED(res)) {
            if (!(FS::IsSaveAccessible(MEDIATYPE_GAME_CARD, currentTitleId)) && !(FS::IsSaveAccessible(MEDIATYPE_SD, currentTitleId))) {
                MsgDisp(top, "Unable to Open the Save Archive\nSave file may not have been created!");
                Editor::Cleanup();
                return ReturnMode::Exit;
            }
        }

        // Initialize a new Save class
        Save* saveFile = Save::Initialize(saveArch, true);

        if (saveFile->GetSaveSize() != SIZE_SAVE) {
            MsgDisp(top, "Save file is the incorrect size!");
            Editor::Cleanup();
            saveFile->Close();
            return ReturnMode::Exit;
        }

        if (Config::Instance()->Auto_SaveBackup) {
            saveBackup(currentTitleId);
        }

        saveFile->FixSaveRegion();		//Update Region of the Save
        saveFile->FixInvalidBuildings(); //Fix any invalid buildings in save

        retMode = Editor::Spawn_MainMenu();
        saveFile->Close();
    }

    Editor::Cleanup();
    return retMode;
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