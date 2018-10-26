#include <3ds.h>
#include <sys/stat.h> //For mkdir
#include "CTRFont.hpp"
#include "save.h"
#include "gfx.h"
#include "config.hpp"
#include "menus.h"
#include "core/updater.hpp"
#include "editor/editor.h"
#include "libs/httpc-curl/httpc.h"

static FontHandle g_acnlFont;
u64 g_tid = 0;
Config *config;
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

    config->Save();
    ExitGFX();
    cfguExit();
    httpc.Exit();
    amExit();
    romfsExit();
}

int main() {
    InitApp();
    config = new Config();
    g_acnlFont = Font::Open("romfs:/ACNL_font.bcfnt");

    if (g_acnlFont->IsLoaded()) {
        MsgDisp(top, "Error: ACNL font failed to open.\n", MsgTypeAccept);
    }

    Text::SetDefaultFont(g_acnlFont);

    hidScanInput();
    u32 keys = (hidKeysDown() | hidKeysHeld());
    if (keys & KEY_SELECT) {
        config->Reset();
    }

    if (config->Auto_Update) {
        if (Updater::Launch()) {
            return 0;
        }
    }

    spawn_main_menu();

    PrepareToCloseApp();
    return 0;
}