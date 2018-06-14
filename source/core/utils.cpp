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

void draw_centered_text(float StartX, float WidthX, float StartY, float HeightY,
                        float scaleX, float scaleY, u32 color, const char* text)
{
    Text Msg(Color(color), text, scaleX, scaleY);
    Msg.CenterInBounds(StartX, StartY, WidthX, HeightY);
    Msg.Draw();
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
        }
    }

    itemDatabase.close();
}

// abort override
void abort(void)
{
    for (;;)
        MsgDisp(top, "Abort has been called");
}

static inline u32   Pow2(u32 x)
{
    if (x <= 64)
        return 64;

    return 1u << (32 - __builtin_clz(x - 1));
}

C2D_Image   ImageDataToC2DImage(u32 *imageData, u32 width, u32 height, GPU_TEXCOLOR colorFormat) {

    u32     widthPow2 = Pow2(width);
    u32     heightPow2 = Pow2(height);
    u32   * buffer = (u32 *)linearAlloc(sizeof(u32) * widthPow2 * heightPow2);

    // Clear buffer
    C3D_SyncMemoryFill(buffer, 0, (u32 *)((u8 *)buffer + (sizeof(u32) * widthPow2 * heightPow2)), BIT(0) | GX_FILL_32BIT_DEPTH, nullptr, 0, nullptr, 0);
    GSPGPU_FlushDataCache(buffer, widthPow2 * heightPow2 * sizeof(u32));

    // Copy Data
    u32 *dst = buffer;
    u32 *src = imageData;

    for (u32 h = height; h > 0; h--)
    {
        memcpy(dst, src, width * sizeof(u32));
        dst += widthPow2;
        src += width;
    }

    GSPGPU_FlushDataCache(buffer, widthPow2 * heightPow2 * sizeof(u32));

    C3D_Tex *tex = new C3D_Tex();
    C3D_TexInit(tex, Pow2(width), Pow2(height), colorFormat);

    tex->param = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR)
        | GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_BORDER) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_BORDER);
    tex->border = 0xFFFFFFFF;

    C3D_SyncMemoryFill((u32 *)tex->data, 0, (u32 *)((u8 *)tex->data + tex->size), BIT(0) | (tex->fmt << 8), nullptr, 0, nullptr, 0);
    C3D_TexFlush(tex);

    C3D_SyncDisplayTransfer(buffer, GX_BUFFER_DIM(tex->width, tex->height), \
        (u32 *)tex->data, GX_BUFFER_DIM(tex->width, tex->height), TEXTURE_TRANSFER_FLAGS);

    C3D_TexFlush(tex);
    linearFree(buffer);

    Tex3DS_SubTexture *subtex = new Tex3DS_SubTexture();
    subtex->width = width;
    subtex->height = height;
    subtex->left = 0.f;
    subtex->top =  1.f;
    subtex->right = (float)width / (float)tex->width;
    subtex->bottom = 1.f - (float)height / (float)tex->height;

    C2D_Image image;
    image.tex = tex;
    image.subtex = subtex;

    return image;
}

void    C2D_ImageDelete(C2D_Image image)
{
    C3D_TexDelete(image.tex);
    delete image.tex;
    delete image.subtex;
}
