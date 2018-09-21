#include <3ds.h>
#include <string>
#include "gfx.h"
#include "nfs.h"
#include "save.h"
#include "utils.h"
#include "config.h"
#include "menus.h"
#include "editor/editor.h"

extern u64 g_tid;
extern NLTK_config config;
extern FS_MediaType currentMediaType;
extern Save saveFile;

u8* g_ItemBin;
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
	// Load Item Database
	loadItemDatabase();
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

	{
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

	saveFile.FixSaveRegion();		//Update Region of the Save
	saveFile.FixInvalidBuildings(); //Fix any invalid buildings (fixes 7dd6 crash caused by ntr plg)

	int mode = spawn_editor_main_menu(&saveFile);

    saveFile.Close();

    if (mode == MODE_GAMESELECT) //Game Select
        goto GameSelect;

    CleanupEditor();
    return mode; //mode is always 0 when exiting the editor
}