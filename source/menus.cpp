#include <3ds.h>
#include <string>
#include <algorithm>
#include "CTRFont.hpp"
#include "InputManager.h"
#include "gfx.h"
#include "utils.h"
#include "config.hpp"
#include "core/nfs.h"
#include "core/keyboard.h"
#include "core/textbox.h"
#include "core/imagebutton.h"
#include "menus.h"

extern FS_MediaType currentMediaType;

void checkIfCardInserted() {
    if (currentMediaType == MEDIATYPE_GAME_CARD && !FS::IsGameCartInserted()) {
        DisplayCardError();
    }
}