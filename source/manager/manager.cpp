#include "common.h"
#include "manager/manager.h"

bool m_ManagerInitiated = false;

void InitManager(void) {
    mkdir(WORKDIR "/TownManager", 777);
    m_ManagerInitiated = true;
}

int manager_main(void) {
    if (!m_ManagerInitiated) {
        InitManager();
    }

GameSelect:
    Handle saveHandle;
    FS_Archive saveArch;

    if (envIsHomebrew()) {
        srvGetServiceHandleDirect(&saveHandle, "fs:USER");
        FSUSER_Initialize(saveHandle);
        fsUseSession(saveHandle);
    }
    else {
        if (config.autoloadprefGame && !(hidKeysDown() & KEY_DOWN) && is_ACNL(config.prefGame)) {
            g_tid = config.prefGame; // TODO: Support media type in config
        }
        else {
            g_tid = spawn_game_select_menu(&currentMediaType);
        }

        if (g_tid == 0) {
            return 0;
        }   
    }

    bool successfullyOpenedArchive = openSaveArchive(&saveArch, g_tid, currentMediaType);
    if (!successfullyOpenedArchive && !(tryOpenSaveArchive(&saveArch, g_tid, &currentMediaType))) {
        infoDisp(top, "Unable to Open the Save Archive\nSave file may not have been created!");
        return 0;
    }

    // Initialize a new Save class
    saveFile = Save(saveArch, &saveHandle, false);

    if (saveFile.GetSaveSize() != SIZE_SAVE) {
        infoDisp(top, "Save file is the incorrect size!");
        saveFile.Close();
        return 0;
    }

    if (config.autosavebackup) {
        saveBackup(&saveFile, g_tid);
    }

    // Update Region of the Save
    saveFile.FixSaveRegion();

    int mode = spawn_manager_main_menu(&saveFile);

    saveFile.Close();

    if (mode == 1) //Game Select
        goto GameSelect;

    return mode; //mode is always 0 when exiting the editor
}