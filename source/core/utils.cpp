#include <3ds.h>
#include <string>
#include <stdarg.h>
#include <citro2d.h>
#include <codecvt>
#include <locale>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include "gfx.h"
#include "nfs.h"
#include "structs.h"
#include "common.h"
#include "utils.h"

extern u64 currentTitleId;

std::map<u16, std::string> g_itemDatabase;
std::map<u16, std::string> g_villagerDatabase;

// TODO: We should probably just use one item database.
std::map<std::string, std::map<u16, std::string>> g_sortedItemDatabase;

bool is_ACNL(u64 tid)
{
    if (tid == JPN_TID || tid == USA_TID || tid == EUR_TID || tid == KOR_TID)
        return true;

    else if (tid == JPN_WA_TID || tid == USA_WA_TID || tid == EUR_WA_TID || tid == KOR_WA_TID)
        return true;

    else return false;
}

void Sleep(u64 nanoseconds)
{
    svcSleepThread(nanoseconds);
}

s64 Seconds(float amount)
{
    return (s64)(amount*1000000000);
}

u32 UTF8_StringSize(std::string str) {
    char* data = str.data();
    u32 len = 0;

    while (*data) {
        len += (*data++ & 0xC0) != 0x80;
    }

    return len;
}

// This is needed for non ASCII characters; https://stackoverflow.com/a/17436539
void UTF8_String_PopBack(std::string& str) {
    while (str.size() > 0)
    {
        char c = str.back();
        str.pop_back();

        // If we found an initial character, we're done
        if ((c & 0xC0) != 0x80)
            break;
    }
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
        FS::ACNL_TitlesInstalled titles = FS::GetInstalledTitles();
        if (titles.Cart_Titles == FS::ACNL_Game::NONE) return false;
        else return true;
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

inline u16 strToU16(std::string str) {
    u16 out;
    std::stringstream ss;
    ss << std::hex << str;
    ss >> out;

    return out;
}

void loadItemDatabase() {
    g_itemDatabase.clear();
    std::string currentLine;
    std::ifstream itemDatabase("romfs:/Items_en.txt", std::ifstream::in);
    std::string itemIdStr;
    std::string itemName;
    std::string sectionName = "None";

    while (std::getline(itemDatabase, currentLine)) {
        if (currentLine.find("//") == 0) {
            sectionName = currentLine.substr(2);
        }
        else if (currentLine.size() > 8 && currentLine.find("//") == std::string::npos) { // confirm we don't have any comments
            itemIdStr = currentLine.substr(2, 4); // skip the 0x hex specifier
            itemName = currentLine.substr(8, currentLine.size() - 9);

            // Convert itemIdStr to a u16
            u16 itemId = strToU16(itemIdStr);

            // Add item to the database
            g_itemDatabase.insert(std::make_pair(itemId, itemName));
            g_sortedItemDatabase[sectionName][itemId] = itemName;
        }
    }

    itemDatabase.close();
}

void LoadVillagerDatabase() {
    g_villagerDatabase.clear();

    std::string currentLine;
    std::ifstream villagerDatabase("romfs:/Villagers_en.txt", std::ifstream::in);

    while (std::getline(villagerDatabase, currentLine)) {
        u16 id = strToU16(currentLine.substr(0, 4));
        std::string name = currentLine.substr(5, currentLine.size() - 6);

        // TODO: Read default things like personality, catchphrases, & furniture.
        g_villagerDatabase.insert(std::make_pair(id, name));
    }

    villagerDatabase.close();
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
