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
extern Save saveFile;

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

    {
        if (config->Auto_loadprefGame && !(hidKeysDown() & KEY_DOWN) && is_ACNL(config->prefGame|0x4000000000000)) {
            g_tid = config->prefGame|0x4000000000000; // TODO: Support media type in config
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
        MsgDisp(top, "Unable to Open the Save Archive\nSave file may not have been created!");
        return 0;
    }

    // Initialize a new Save class
    saveFile = Save(saveArch, &saveHandle, false);

    if (saveFile.GetSaveSize() != SIZE_SAVE) {
        MsgDisp(top, "Save file is the incorrect size!");
        saveFile.Close();
        return 0;
    }

    if (config->Auto_SaveBackup) {
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