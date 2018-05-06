#include <codecvt>
#include <locale>
#include <memory>
#include "utils.h"

cursorinfo_s g_cursorpos;
LoadItem_s g_InvItems[16];

/*
	bool IsSDCardInserted(void)
		=> returns whether or not the SD Card is inserted
*/
bool IsSDCardInserted() {
	bool inserted = false;
	FSUSER_IsSdmcDetected(&inserted);
	return inserted;
}

/*
	bool IsGameCartInserted(void)
		=> returns whether or not a GameCard is inserted
*/
bool IsGameCartInserted() {
	bool inserted = false;
	FSUSER_CardSlotIsInserted(&inserted);
	return inserted;
}

int    RandomNum(int start, int end)
{
    srand(svcGetSystemTick());
    int r[20];

    for (int i = 0; i < 20; i++)
        r[i] = rand() % (end - start + 1) + start;

    return (r[rand() % 20]);
}

//Credit to SciresM for this code! :)
u32     DecryptACNLValue(u64 encval)
{
    // Unpack 64-bit value into (u32, u16, u8, u8) values.
    u32 enc = (encval & 0xFFFFFFFF);
    u16 adjust = ((encval >> 32) & 0xFFFF);
    u8  shift_val = ((encval >> 48) & 0xFF);
    u8  chk = ((encval >> 56) & 0xFF);
    // Validate 8-bit checksum
    if ((((enc >> 0) + (enc >> 8) + (enc >> 16) + (enc >> 24) + 0xBA) & 0xFF) != chk) return 0;
        
    u8  left_shift = ((0x1C - shift_val) & 0xFF);
    u8  right_shift = 0x20 - left_shift;

    // Handle error case: Invalid shift value.
    if (left_shift >= 0x20)
    {
        return 0 + (enc << right_shift) - (adjust + 0x8F187432);
    }

    // This case should occur for all game-generated values.
    return (enc << left_shift) + (enc >> right_shift) - (adjust + 0x8F187432);
}

u64     EncryptACNLValue(u32 decval)
{
    // Make a new RNG
    u16 adjust = RandomNum(0, 0x10000);
    u8  shift_val = RandomNum(0, 0x1A);

    // Encipher value
    u32 enc = decval + adjust + 0x8F187432;
    enc = (enc >> (0x1C - shift_val)) + (enc << (shift_val + 4));

    // Calculate Checksum
    u8  chk = (((enc >> 0) + (enc >> 8) + (enc >> 16) + (enc >> 24) + 0xBA) & 0xFF);

    // Pack result
    return ((u64)enc << 0) | ((u64)adjust << 32) | ((u64)shift_val << 48) | ((u64)chk << 56);
}

char Sstrncpy(char *dest, const char *src, size_t n) //'Safe' strncpy, always null terminates
{
    strncpy(dest, src, n);
    return dest[n] = '\0';
}

bool is_ACNL(u64 tid)
{
    if (tid == JPN_TID || tid == USA_TID || tid == EUR_TID || tid == KOR_TID)
        return true;

    else if (tid == JPN_WA_TID || tid == USA_WA_TID || tid == EUR_WA_TID || tid == KOR_WA_TID)
        return true;

    else return false;
}

bool DebugCode(bool complete)
{
    static const u32 debug_code[] =
    {
        KEY_X, KEY_Y, KEY_X, KEY_Y, KEY_UP,
        KEY_RIGHT, KEY_DOWN, KEY_LEFT, KEY_X, KEY_A
    };

    const u32 maxpos = sizeof(debug_code)/sizeof(debug_code[0]);
    static u32 pos = 0;
    static u32 timeout = 60;
    u32 down = hidKeysDown();

    if (!complete)
    {
        if (timeout > 0)
            timeout--;

        if (timeout == 0)
            pos = 0;

        if (pos == maxpos)
        {
            pos = 0;
            timeout = 30;
            return true;
        }

        else if (down)
        {
            pos = (down & debug_code[pos]) ? pos+1 : 0;
            timeout = 30;
        }
    }
    return false;
}

void Sleep(u64 nanoseconds)
{
    svcSleepThread(nanoseconds);
}

s64 Seconds(float amount)
{
    return (s64)(amount*1000000000);
}

void load_player_invitems(Save *saveFile , int selectedplayer)
{
    for (int num = 0; num < 16; num++)
    {
		Item item = saveFile->players[selectedplayer].Pockets[num];

		GetIconID(&item, &g_InvItems[num]); // Should probably rename this to "GetIconData"
		g_InvItems[num].ItemName = item.Name;
    }
}

float getCenterText_X(const char* text, float scaleX, float scaleY, float StartX, float WidthX)
{
    float CharWidth = pp2d_get_text_width(text, scaleX, scaleY);
    float CenterX = StartX+(WidthX - CharWidth) /2;
    return CenterX;
}

float getCenterText_Y(const char* text, float scaleX, float scaleY, float StartY, float HeightY)
{
    float CharHeight = pp2d_get_text_height(text, scaleX, scaleY);
    float CenterY = StartY+(HeightY-CharHeight) /2;
    return CenterY;
}

void draw_centered_text(float StartX, float WidthX, float StartY, float HeightY,
                        float scaleX, float scaleY, u32 color, const char* text)
{
    float XCoord = getCenterText_X(text, scaleX, scaleY, StartX, WidthX);
    float YCoord = getCenterText_Y(text, scaleX, scaleY, StartY, HeightY);
    pp2d_draw_text(XCoord, YCoord, scaleX, scaleY, color, text);
}

void draw_centered_textf(float StartX, float WidthX, float StartY, float HeightY,
                        float scaleX, float scaleY, u32 color, const char* string, ...)
{
    char       buffer[1024];
    va_list    args;
    va_start(args, string);
    vsnprintf(buffer, sizeof(buffer), string, args);
    draw_centered_text(StartX, WidthX, StartY, HeightY, scaleX, scaleY, color, buffer);
    va_end(args);
}

void updatePointerPosition(void)
{
    hidScanInput();

    if (hidKeysHeld() & KEY_A)
    {
        g_cursorpos.A_held = true;
        return;
    }

    else
        g_cursorpos.A_held = false;
    //CPad
    if (hidKeysHeld() & KEY_CPAD)
    {
        if (hidKeysHeld() & KEY_CPAD_RIGHT)
            g_cursorpos.x += 3;
        
        if (hidKeysHeld() & KEY_CPAD_LEFT)
            g_cursorpos.x -= 3;
        
        if (hidKeysHeld() & KEY_CPAD_UP)
            g_cursorpos.y -= 3;
        
        if (hidKeysHeld() & KEY_CPAD_DOWN)
            g_cursorpos.y += 3;

        g_cursorpos.show = true;
    }

    //C-Stick
    if (hidKeysHeld() & KEY_CSTICK)
    {
        if (hidKeysHeld() & KEY_CSTICK_RIGHT)
            g_cursorpos.x += 4;
        
        if (hidKeysHeld() & KEY_CSTICK_LEFT)
            g_cursorpos.x -= 4;
        
        if (hidKeysHeld() & KEY_CSTICK_UP)
            g_cursorpos.y -= 4;
        
        if (hidKeysHeld() & KEY_CSTICK_DOWN)
            g_cursorpos.y += 4;

        g_cursorpos.show = true;
    }

    if (hidKeysHeld() & KEY_TOUCH)
        g_cursorpos.show = false;

    if (g_cursorpos.y < -5)
        g_cursorpos.y = 235;

    if (g_cursorpos.y > 235)
        g_cursorpos.y = -5;

    if (g_cursorpos.x < -5)
        g_cursorpos.x = 315;

    if (g_cursorpos.x > 315)
        g_cursorpos.x = -5;
}

void updateCursorInfo(void)
{
    touchPosition touch;
    hidTouchRead(&touch);
    
    g_CheckX[0] = touch.px;
    g_CheckY[0] = touch.py;
    g_CheckX[1] = g_cursorpos.x;
    g_CheckY[1] = g_cursorpos.y;
    g_disabled[0] = !g_cursorpos.show;
    g_disabled[1] = g_cursorpos.show;
}

bool CheckUnreadMail(int selectedplayer)
{
    /* STUB until player struct finished*/
	if (selectedplayer == 0) { // TODO: REMOVE THIS (It's here to stop compiler warnings)
		return true;
	}
    return true;
}

Item GetItemFromSave(Save *saveFile, u32 offset)
{
    Item item;
    item.Id = saveFile->ReadU16(offset);
    item.Flag1 = saveFile->ReadU16(offset + 2);
	item.Flag2 = saveFile->ReadU16(offset + 3);
    return item;
}

int CheckItemValidity(Item *item)
{
    //Real Max ID is 0x372B (exclude wrap paper) but for checks game -= 0x2000 from orig item ID
    static const s16 maxID = 0x172B;
    int chk;

    u16 ID = item->Id & ~0x8000;
    chk = ID - 0x2000; //To cover items lower than 0x2000 (Enviroment Items)

    if (chk < maxID)
    return chk;

    else return -1;
}

u32 CheckIfValidItem(Item *item)
{
    static const u16 maxID = 0x172B;
    int chk = CheckItemValidity(item);

    if (chk < 0 || chk > maxID)
        return 0;

    else /* chk <= maxID */
    {
        chk = ((chk << 4) - chk) << 1;
        return chk;
    }
}

u16 GetAxeDamageValue(Item *item)
{
    /* Used for Normal Axe and Silver Axe */
    if (item->Id != 0x334D && item->Id != 0x334E)
        return 0;

    return (item->Flag2 & 0xF); // TODO: Confirm Flag2 is correct
}

u16 GetAxeDamageIcon(Item *item, u16 ItemIcon)
{
    static u8 axedamagetable[] = {0, 0, 0, 1, 1, 1, 2, 2};
    u16 DamageValue = GetAxeDamageValue(item);
    if (DamageValue >= 8)
        return ItemIcon;

    return ItemIcon + axedamagetable[DamageValue];
}

u8 GetItemCategory(Item *item)
{
    if (g_ItemBin == NULL)
        return 0;

    u32 offset = CheckIfValidItem(item);
    if (offset == 0)
        return 0x9B;

    u8 category = g_ItemBin[offset+0xA];
    if (category >= 0x9B)
        return 0;

    return category;
}

u16 GetIconID(Item *item, LoadItem_s *itemData)
{
	// Set icon default to blank icon
	itemData->X = 9 * 32;
	itemData->Y = 24 * 32;

    if (g_ItemBin == NULL)
        return 0;

    u32 offset = CheckIfValidItem(item);
    if (offset == 0)
        return 0;

    u16 iconID = (g_ItemBin[offset + 1] << 8) + g_ItemBin[offset]; //Reading from BE file; 2nd u8 comes first to convert to LE
	if (iconID >= 0x1FB) {
		iconID = 0;
	}

    u8 category = GetItemCategory(item);
	if (category == 0xE && (item->Id == 0x334D || item->Id == 0x334E)) {
		iconID = GetAxeDamageIcon(item, iconID);
	}

	itemData->X = (iconID % 16) * 32;
	itemData->Y = (iconID / 16) * 32;

    return iconID;
}

std::string GetItemName(Item *item)
{
    char itemid[10];
    char* line = new char[100];
    snprintf(itemid, 8, "0x%04X", item->Id);

    rewind(g_Items_en);
    while (fgets(line, 100, g_Items_en) != NULL) /* read each line */
    {
        if (strncmp(itemid, line, 6) == 0) /* If itemID in txt is same as our ID */
        {
            int len = strlen(line + 8) - 1;
            if ((len > 0) && (line[len] == '\n'))
            line[len] = '\0';
			auto str = std::string(line + 8); // +8 so the itemID and spaces before actual name aren't included
			delete[] line;
            return str;
        }
    }

	delete[] line;
	return std::string("???");
}

std::string u16tou8(std::u16string src)
{
	static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	std::string dst = convert.to_bytes(src);
	return dst;
}

std::u16string u8tou16(const char* src)
{
	char16_t tmp[256] = { 0 };
	utf8_to_utf16((uint16_t *)tmp, (uint8_t *)src, 256);
	return std::u16string(tmp);
}

/*
	bool checkGameCartIsACNL
		=> Returns whether or not the game cart is an ACNL game cart.
*/
bool checkGameCartIsACNL() {
	if (currentTitleId == 0) {
		return getInstalledTitles(MEDIATYPE_GAME_CARD).HasACNLData;
	}
	else {
		return checkGameCartTitleSame(currentTitleId);
	}
}