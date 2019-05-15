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
#include "config.hpp"
#include "gfx.h"
#include "nfs.h"
#include "structs.h"
#include "common.h"
#include "utils.h"

extern u64 currentTitleId;

std::map<u16, std::string> g_villagerDatabase;

bool is_ACNL(u64 tid)
{
    if (tid == JPN_TID || tid == USA_TID || tid == EUR_TID || tid == KOR_TID)
        return true;

    else if (tid == JPN_WA_TID || tid == USA_WA_TID || tid == EUR_WA_TID || tid == KOR_WA_TID)
        return true;

    else return false;
}

void Sleep(s64 nanoseconds)
{
    svcSleepThread(nanoseconds);
}

s64 Seconds(float amount)
{
    return (s64)(amount*1000000000);
}

s64 MicroSeconds(float amount)
{
    return (s64)(amount*1000000);
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
    char tmp[256] = {0};
    utf16_to_utf8((uint8_t *)tmp, (uint16_t *)src.data(), 256);
    return std::string(tmp);
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

u16 strToU16(std::string str) {
    u16 out;
    std::stringstream ss;
    ss << std::hex << str;
    ss >> out;

    return out;
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

    tex->param = GPU_TEXTURE_MAG_FILTER(GPU_NEAREST) | GPU_TEXTURE_MIN_FILTER(GPU_NEAREST)
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

bool Utils_IsNew3DS(void) {
    bool isNew3DS = false;
    if (R_SUCCEEDED(APT_CheckNew3DS(&isNew3DS))) {
        return isNew3DS;
    }

    return false;
}

bool SetupAutoLoad(u64 &TID, FS_MediaType &MediaType) {
    switch (Config::Instance()->prefGame) {
        case FS::ORIG_JPN:
            TID = JPN_TID;
            break;
        case FS::ORIG_USA:
            TID = USA_TID;
            break;
        case FS::ORIG_EUR:
            TID = EUR_TID;
            break;
        case FS::ORIG_KOR:
            TID = KOR_TID;
            break;
        case FS::WA_JPN:
            TID = JPN_WA_TID;
            break;
        case FS::WA_USA:
            TID = USA_WA_TID;
            break;
        case FS::WA_EUR:
            TID = EUR_WA_TID;
            break;
        case FS::WA_KOR:
            TID = KOR_WA_TID;
            break;
        default:
            return false;
    }

    switch (Config::Instance()->prefGameMediaType) {
        case 1:
            MediaType = MEDIATYPE_SD;
            break;
        case 2:
            MediaType = MEDIATYPE_GAME_CARD;
            break;
        default:
            return false;
    }

    return true;
}

void SaveAutoLoad(u64 TID, FS_MediaType MediaType) {
    u8 Game = 0;
    u8 MT = 0;
    switch (TID) {
        case JPN_TID:
            Game = FS::ORIG_JPN;
            break;
        case USA_TID:
            Game = FS::ORIG_USA;
            break;
        case EUR_TID:
            Game = FS::ORIG_EUR;
            break;
        case KOR_TID:
            Game = FS::ORIG_KOR;
            break;
        case JPN_WA_TID:
            Game = FS::WA_JPN;
            break;
        case USA_WA_TID:
            Game = FS::WA_USA;
            break;
        case EUR_WA_TID:
            Game = FS::WA_EUR;
            break;
        case KOR_WA_TID:
            Game = FS::WA_KOR;
            break;

        default:
            Game = 0;
            break;
    }

    switch (MediaType) {
        case MEDIATYPE_SD:
            MT = 1;
            break;
        case MEDIATYPE_GAME_CARD:
            MT = 2;
            break;
        default:
            MT = 0;
            break;
    }

    if (Game == 0 || MT == 0) {
        MT = 0;
        Game = 0;
    }

    Config::Instance()->prefGame = Game;
    Config::Instance()->prefGameMediaType = MT;
}

