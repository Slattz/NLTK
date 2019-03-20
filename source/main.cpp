#include <3ds.h>
#include "CTRFont.hpp"
#include "save.h"
#include "nfs.h"
#include "gfx.h"
#include "utils.h"
#include "config.hpp"
#include "menus.h"
#include "core/updater.hpp"
#include "editor/editor.h"
#include "libs/httpc-curl/httpc.h"
#include "InputManager.h"

static FontHandle g_acnlFont;
u64 g_tid = 0;

Result InitApp(void) {
    //Check if using Rosalina or Old Hax HB
    s64 out;
    if (envIsHomebrew() && R_FAILED(svcGetProcessInfo(&out, CUR_PROCESS_HANDLE, 0x10001))) {
        return -1; //Old Hax HB isn't supported
    }

    if (Utils_IsNew3DS())
        osSetSpeedupEnable(true);

    romfsInit();
    amInit();
    cfguInit();
    httpc.Init(0);
    FS::Initialize();
    InitGFX();

    return 0;
}

void PrepareToCloseApp(void) {
    Config::Instance()->Save();
    FS::Cleanup();
    ExitGFX();
    httpc.Exit();
    cfguExit();
    amExit();
    romfsExit();

    if (Utils_IsNew3DS())
        osSetSpeedupEnable(false);
}

int main() {
    if (InitApp() == -1) { //Old Hax HB isn't supported
        return 0; //No services initilized so no need to uninit
    }

    g_acnlFont = Font::Open("romfs:/ACNL_font.bcfnt");

    if (g_acnlFont->IsLoaded()) {
        MsgDisp(top, "Error: ACNL font failed to open.\n", MsgTypeAccept);
    }

    Text::SetDefaultFont(g_acnlFont);

    InputManager::Instance()->RefreshInput();
    if (InputManager::Instance()->IsButtonActive(KEY_SELECT)) {
        Config::Instance()->Reset();
    }

    if (Config::Instance()->Auto_Update) {
        if (Updater::Launch()) {
            return 0;
        }
    }

    spawn_main_menu();

    PrepareToCloseApp();
    return 0;
}