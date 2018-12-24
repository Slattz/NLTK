#include <3ds.h>
#include <sys/stat.h> //For mkdir
#include "gfx.h"
#include "nfs.h"
#include "save.h"
#include "utils.h"
#include "config.hpp"
#include "structs.h"
#include "menus.h"
#include "manager/manager.h"

extern Config *config;
extern u64 g_tid;
extern FS_MediaType currentMediaType;

bool m_ManagerInitiated = false;

void InitManager(void) {
    mkdir(WORKDIR "/TownManager", 777);
    m_ManagerInitiated = true;
}

ReturnMode manager_main(void) {
    if (!m_ManagerInitiated) {
        InitManager();
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
            return ReturnMode::Exit;
        }
    }

    bool successfullyOpenedArchive = openSaveArchive(&saveArch, g_tid, currentMediaType);
    if (!successfullyOpenedArchive && !(tryOpenSaveArchive(&saveArch, g_tid, &currentMediaType))) {
        MsgDisp(top, "Unable to Open the Save Archive\nSave file may not have been created!");
        return ReturnMode::Exit;
    }

    // Initialize a new Save class
    Save* saveFile = Save::Initialize(saveArch, &saveHandle, false);

    if (saveFile->GetSaveSize() != SIZE_SAVE) {
        MsgDisp(top, "Save file is the incorrect size!");
        saveFile->Close();
        return ReturnMode::Exit;
    }

    if (config->Auto_SaveBackup) {
        saveBackup(g_tid);
    }

    // Update Region of the Save
    saveFile->FixSaveRegion();

    ReturnMode mode = spawn_manager_main_menu();

    saveFile->Close();

    if (mode == ReturnMode::GameSelect) //Game Select
        goto GameSelect;

    return mode; //mode is always 0 when exiting the editor
}