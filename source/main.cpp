#include "common.h"
#include "core/updater.h"
#include "editor/editor.h"
#include "libs/httpc-curl/httpc.h"

static BCFNT_s *g_acnlFont;

void InitApp(void) {
    romfsInit();
	mkdir(WORKDIR, 777);
    amInit();
    httpc.Init(0);
    pp2d_init();
	cfguInit();
	InitCommonGFX();
    g_cursorpos.x = 160;
    g_cursorpos.y = 120;
}

void PrepareToCloseApp(void) {
	if (envIsHomebrew()) {
		fsEndUseSession();
	}

    saveConfig();
	ExitCommonGFX();
	cfguExit();
    pp2d_exit();
    httpc.Exit();
    amExit();
    romfsExit();
}

int main() {
    InitApp();
	g_acnlFont = (BCFNT_s *)malloc(sizeof(BCFNT_s));

	if (R_SUCCEEDED(bcfnt_load_font(g_acnlFont, "romfs:/ACNL_font.bcfnt"))) {
		pp2d_use_font(g_acnlFont);
	}
    else {
        infoDisp(GFX_TOP, "Error: Font load failed.\nUsing System Font.");
        delete g_acnlFont;
    }

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

    editor_main();

    PrepareToCloseApp();
    return 0;
}