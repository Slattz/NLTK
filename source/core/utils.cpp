#include <codecvt>
#include <locale>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include "utils.h"

cursorinfo_s g_cursorpos;
std::map<u16, std::string> g_itemDatabase;

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

float CenterTextX(Text msg, float StartX, float WidthX)
{
    float CharWidth = msg.GetWidth();
    float CenterX = StartX+(WidthX - CharWidth) /2;
    return CenterX;
}

float CenterTextY(Text msg, float StartY, float HeightY)
{
    float CharHeight = msg.GetHeight();
    float CenterY = StartY+(HeightY-CharHeight) /2;
    return CenterY;
}

void draw_centered_text(float StartX, float WidthX, float StartY, float HeightY,
                        float scaleX, float scaleY, u32 color, const char* text)
{
    Text Msg(Color(color), text, scaleX, scaleY);
    float XCoord = CenterTextX(Msg, StartX, WidthX);
    float YCoord = CenterTextY(Msg, StartY, HeightY);
    Msg.Draw(XCoord, YCoord);
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

std::string Format(const char* fmt, ...)
{
    char        buffer[0x100] = { 0 };
    va_list     argList;

    va_start(argList, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, argList);
    va_end(argList);

    return (std::string(buffer));
}

void loadItemDatabase() {
	g_itemDatabase.clear();
	std::string currentLine;
	std::ifstream itemDatabase("romfs:/Items_en.txt", std::ifstream::in);
	std::string itemIdStr;
	std::string itemName;

	while (std::getline(itemDatabase, currentLine)) {
		if (currentLine.size() > 8 && currentLine.find("//") == std::string::npos) { // confirm we don't have any comments
			itemIdStr = currentLine.substr(2, 4); // skip the 0x hex specifier
			itemName = currentLine.substr(8, currentLine.size() - 9);
			u16 itemId = 0;
			
			// Convert itemIdStr to a u16
			std::stringstream ss;
			ss << std::hex << itemIdStr;
			ss >> itemId;

			// Add item to the database
			g_itemDatabase.insert(std::make_pair(itemId, itemName));

			// TESTING
			//MsgDisp(top, Format("Item ID: %04X\nItem Name: %s", itemId, itemName.c_str()));
		}
	}

	itemDatabase.close();
}

// abort override
void abort(void)
{
	MsgDisp(top, "Abort has been called");
	for (;;);
}