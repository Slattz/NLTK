#include "common.h"
#include "editor/editor.h"

u8* g_ItemBin;
FILE* g_Items_en;
u64 currentTitleId;
bool m_editorInitiated = false;

void CleanupEditor(void) {
	if (g_ItemBin) {
		delete[] g_ItemBin;
		g_ItemBin = NULL;
	}
    m_editorInitiated = false;
}

void init_editor(void) {
	g_Items_en = fopen("romfs:/Items_en.txt", "r");
	InitEditorGFX();

	g_ItemBin = new u8[0x2B720];
	file_read(g_ItemBin, "romfs:/Item.bin", 0x2B720);

	m_editorInitiated = true;
}

int editor_main(void) {
	if (!m_editorInitiated) {
		init_editor();
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
			CleanupEditor();
            return 0;
        }   
    }

	bool successfullyOpenedArchive = openSaveArchive(&saveArch, g_tid, currentMediaType);
    if (!successfullyOpenedArchive && !(tryOpenSaveArchive(&saveArch, g_tid, &currentMediaType))) {
        MsgDisp(top, "Unable to Open the Save Archive\nSave file may not have been created!");
		CleanupEditor();
        return 0;
    }

	// Set current title id
	currentTitleId = g_tid;

	// Initialize a new Save class
	saveFile = Save(saveArch, &saveHandle, true);

	if (saveFile.GetSaveSize() != SIZE_SAVE) {
		MsgDisp(top, "Save file is the incorrect size!");
		CleanupEditor();
		saveFile.Close();
		return 0;
	}

	if (config.autosavebackup) {
		saveBackup(&saveFile, g_tid);
	}

    // Update Region of the Save
    saveFile.FixSaveRegion();

    int mode = spawn_editor_main_menu(&saveFile);

    saveFile.Close();

    if (mode == MODE_GAMESELECT) //Game Select
        goto GameSelect;

    CleanupEditor();
    return mode; //mode is always 0 when exiting the editor
}