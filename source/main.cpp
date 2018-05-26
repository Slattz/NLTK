#include "common.h"
#include "core/updater.h"
#include "editor/editor.h"
#include "libs/httpc-curl/httpc.h"

static FontHandle g_acnlFont;
u64 g_tid = 0;
Save saveFile;
FS_MediaType currentMediaType;

void InitApp(void) {
    romfsInit();
	mkdir(WORKDIR, 777);
    amInit();
    httpc.Init(0);
	cfguInit();
	InitGFX();
    g_cursorpos.x = 160;
    g_cursorpos.y = 120;
}

void PrepareToCloseApp(void) {
	if (envIsHomebrew()) {
		fsEndUseSession();
	}

    saveConfig();
	ExitGFX();
	cfguExit();
    httpc.Exit();
    amExit();
    romfsExit();
}

int main() {
    InitApp();
	g_acnlFont = Font::Open("romfs:/ACNL_font.bcfnt");

	if (g_acnlFont->IsLoaded()) {
        infoDisp(top, "Error: ACNL font failed to open.\n");
    }

    Text::SetDefaultFont(g_acnlFont);

    hidScanInput();
    u32 keys = (hidKeysDown() | hidKeysHeld());
	if (keys & KEY_SELECT) {
		resetConfig();
	}

    configInit();

    if (config.autoupdate) {
		if (launchUpdater()) {
			return 0;
		}
    }

    spawn_main_menu();

    PrepareToCloseApp();
    return 0;
}