#include <3ds.h>
#include <sys/stat.h> //For mkdir
#include "CTRFont.hpp"
#include "save.h"
#include "gfx.h"
#include "oldconfig.h"
#include "menus.h"
#include "core/updater.h"
#include "editor/editor.h"
#include "libs/httpc-curl/httpc.h"

extern NLTK_config config;

static FontHandle g_acnlFont;
u64 g_tid = 0;
Save saveFile;
FS_MediaType currentMediaType;

// Override ctrulib appInit to check for Rosalina hbl
void __appInit(void)
{
    // Initialize services
    srvInit();
    aptInit();
    hidInit();

    fsInit();

    // Check if we're using Rosalina or not
    s64 out;
    if (envIsHomebrew() && R_FAILED(svcGetProcessInfo(&out, CUR_PROCESS_HANDLE, 0x10001)))
    {
        Handle fsuHandle;
        srvGetServiceHandleDirect(&fsuHandle, "fs:USER");
        FSUSER_Initialize(fsuHandle);
        fsUseSession(fsuHandle);
    }

    sdmcInit();
}

void InitApp(void) {
    romfsInit();
    mkdir(WORKDIR, 777);
    amInit();
    httpc.Init(0);
    cfguInit();
    InitGFX();
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
        MsgDisp(top, "Error: ACNL font failed to open.\n", MsgTypeAccept);
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