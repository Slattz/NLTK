#include <3ds.h>
#include "gfx.h"
#include "nfs.h"
#include "save.h"
#include "utils.h"
#include "config.hpp"
#include "structs.h"
#include "menus.h"
#include "manager/manager.h"
#include "core/gui/GameSelectMenu.hpp"

u64 g_tid;
extern FS_MediaType currentMediaType;

bool m_ManagerInitiated = false;

void InitManager(void) {
    m_ManagerInitiated = true;
}

ReturnMode manager_main(void) {
    ReturnMode retMode = ReturnMode::None;
    if (!Config::Instance()->IsDebug) return ReturnMode::MainMenu;

    if (!m_ManagerInitiated) {
        InitManager();
    }

    while (retMode == ReturnMode::GameSelect || retMode == ReturnMode::None) {
        FS_Archive saveArch;

        InputManager::Instance()->RefreshInput();
        if (Config::Instance()->Auto_loadprefGame && !InputManager::Instance()->IsButtonDown(KEY_DOWN) && is_ACNL(Config::Instance()->prefGame|0x4000000000000)) {
            g_tid = Config::Instance()->prefGame|0x4000000000000; // TODO: Support media type in config
        }
        else {
            g_tid = Core::Spawn_GameSelectMenu(currentMediaType);
        }

        if (g_tid == 0) {
            return ReturnMode::Exit;
        }

        Result res = FS::OpenSaveArchive(&saveArch, currentMediaType, g_tid);
        if (R_FAILED(res)) {
            if (!(FS::IsSaveAccessible(MEDIATYPE_GAME_CARD, g_tid)) && !(FS::IsSaveAccessible(MEDIATYPE_SD, g_tid))) {
                MsgDisp(top, "Unable to Open the Save Archive\nSave file may not have been created!");
                return ReturnMode::Exit;
            }
        }

        // Initialize a new Save class
        Save* saveFile = Save::Initialize(saveArch, false);

        if (saveFile->GetSaveSize() != SIZE_SAVE) {
            MsgDisp(top, "Save file is the incorrect size!");
            saveFile->Close();
            return ReturnMode::Exit;
        }

        if (Config::Instance()->Auto_SaveBackup) {
            saveBackup(g_tid);
        }

        // Update Region of the Save
        saveFile->FixSaveRegion();

        retMode = spawn_manager_main_menu();

        saveFile->Close();
    }

    return retMode; //mode is always 0 when exiting the editor
}