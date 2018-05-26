#pragma once

#ifndef COMMON_H
#define COMMON_H

#include <3ds.h>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <stdarg.h>
#include <citro2d.h>


#include "CTRFont.hpp"
#include "structs.h"
#include "core/config.h"
#include "editor/encryptedInt32.h"
#include "core/nfs.h"
#include "gfx.h"
#include "editor/jpeg.h"
#include "menus.h"
#include "editor/e_menus.h"
#include "item.h"
#include "offsets.h"
#include "player.h"
#include "save.h"
#include "core/swkbd.h"
#include "unicode.h"
#include "utils.h"

extern Save saveFile;
extern u64 g_tid;
extern u8* g_ItemBin;
extern FILE* g_Items_en;
extern NLTK_Titles_Info MediaInfo;
extern cursorinfo_s g_cursorpos;
extern s16  g_CheckX[2];
extern s16  g_CheckY[2];
extern bool g_disabled[2];
extern u32  g_key[2];
extern LoadItem_s g_InvItems[16];
extern NLTK_config config;
extern u64 currentTitleId;
extern FS_MediaType currentMediaType;

static u64 *g_titleIds = new u64[8]{
	0x0004000000086200, // New Leaf Japan
	0x0004000000086300, // New Leaf USA
	0x0004000000086400, // New Leaf Europe
	0x0004000000086500, // New Leaf Korea

	0x0004000000198D00, // Welcome Amiibo Japan
	0x0004000000198E00, // Welcome Amiibo USA
	0x0004000000198F00, // Welcome Amiibo Europe
	0x0004000000199000  // Welcome Amiibo Korea
};

#define STRINGIFY(x)   #x
#define TOSTRING(x)    STRINGIFY(x)

#define MAJOR_VERSION  0
#define MINOR_VERSION  2
#define REV_VERSION    6
#define BETA_VERSION   0
#define VERSION        "v" TOSTRING(MAJOR_VERSION) "." TOSTRING(MINOR_VERSION) "." TOSTRING(REV_VERSION)
#define WORKDIR        "sdmc:/NLTK"
#define TITLE          "NLTK"

//Title IDs
#define JPN_TID     0x0004000000086200
#define USA_TID     0x0004000000086300
#define EUR_TID     0x0004000000086400
#define KOR_TID     0x0004000000086500

#define JPN_WA_TID  0x0004000000198D00
#define USA_WA_TID  0x0004000000198E00
#define EUR_WA_TID  0x0004000000198F00
#define KOR_WA_TID  0x0004000000199000

#define ACRE_ID_MAX 214

enum EXTRA_HID
{
    KEY_CPAD = KEY_CPAD_DOWN | KEY_CPAD_UP | KEY_CPAD_LEFT | KEY_CPAD_RIGHT,
    KEY_CSTICK = KEY_CSTICK_DOWN | KEY_CSTICK_UP | KEY_CSTICK_LEFT | KEY_CSTICK_RIGHT
};

#endif 