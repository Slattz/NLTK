#include <3ds.h>
#include <string>
#include <cstring>
#include <vector>
#include <array>
#include <algorithm>
#include <time.h>
#include "gfx.h"
#include "file.hpp"
#include "structs.h"
#include "utils.h"
#include "save.h"
#include "common.h"
#include "nfs.h"

static Result SetACNLTitlesOnCart(void);
static Result SetACNLTitlesOnSD(void);

FS_MediaType currentMediaType;
static FS::ACNL_TitlesInstalled InstalledTitles;
static FS_Archive sdmcArch = 0;

static constexpr std::array<u64, 8> ACNLTitleIDs = {
    0x0004000000086200, //Orig JPN
    0x0004000000086300, //Orig USA
    0x0004000000086400, //Orig EUR
    0x0004000000086500, //Orig KOR

    0x0004000000198D00, //WA JPN
    0x0004000000198E00, //WA USA
    0x0004000000198F00, //WA EUR
    0x0004000000199000, //WA KOR
};

Result FS::Initialize(void) {
    Result res = 0;

    if (R_FAILED(res = FSUSER_OpenArchive(&sdmcArch, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""))))
        return res;

    CreateDir(WORKDIR);
    CreateDir(WORKDIR "/Saves");
    CreateDir(WORKDIR "/TownManager");

    if (R_FAILED(res = SetACNLTitlesOnCart())) return res;
    if (R_FAILED(res = SetACNLTitlesOnSD())) return res;

    return res;
}


void FS::Cleanup(void) {
    FSUSER_CloseArchive(sdmcArch);
}

Result FS::CreateDir(const std::string &path) {
    return FSUSER_CreateDirectory(GetSDMCArchive(), fsMakePath(PATH_ASCII, path.c_str()), 0);
}

FS_Archive FS::GetSDMCArchive(void) {
    return sdmcArch;
}

Result FS::OpenSaveArchive(FS_Archive *archive, FS_MediaType MediaType, u64 TitleID) {
        u32 path[3] = {MediaType, static_cast<u32>(TitleID), static_cast<u32>(TitleID >> 32)};
        FS_Path TitlePath = {PATH_BINARY, 12, path};
        return FSUSER_OpenArchive(archive, ARCHIVE_USER_SAVEDATA, TitlePath);
}

bool FS::IsSaveAccessible(FS_MediaType MediaType, u64 TitleID) {
    FS_Archive archive;
    Result res = OpenSaveArchive(&archive, MediaType, TitleID);
    if (R_SUCCEEDED(res)) {
        FSUSER_CloseArchive(archive);
        return true;
    }
    return false;
}

static Result SetACNLTitlesOnCart(void) {
    Result res = 0;
    u32 amount = 0;
    u64 TitleID = 0;
    InstalledTitles.Cart_Titles = FS::ACNL_Game::NONE; //Set ACNL_Game values

    if (!FS::IsGameCartInserted()) return res;

    AM_GetTitleCount(MEDIATYPE_GAME_CARD, &amount); //Get amount of titles on cart (should only be 1)
    if (R_SUCCEEDED(res) && amount == 1) {
        res = AM_GetTitleList(NULL, MEDIATYPE_GAME_CARD, 1, &TitleID); //Get all installed title ids
        if (R_FAILED(res)) return res;

        for(u32 i = 0; i < ACNLTitleIDs.size(); i++)
        {
            u64 ID = ACNLTitleIDs.at(i);
            if (ID == TitleID) {
                InstalledTitles.Cart_Titles |= (1 << i); //Set ACNL_Game values
                break; //Only 1 on cart, so no need to go further in loop
            }
        }
    }
    return res;
}

static Result SetACNLTitlesOnSD(void) {
    Result res = 0;
    u32 amount = 0;
    InstalledTitles.SD_Titles = FS::ACNL_Game::NONE; //Set ACNL_Game values

    if (!FS::IsSDCardInserted()) return res;

    res = AM_GetTitleCount(MEDIATYPE_SD, &amount); //Get how many titles are installed on SD
    if (R_SUCCEEDED(res) && amount > 0) {
        std::vector<u64> titles(amount);
        u64 *ptr = titles.data();
        res = AM_GetTitleList(NULL, MEDIATYPE_SD, amount, ptr); //Get all installed title ids
        if (R_FAILED(res)) {
            titles.clear();
            return res;
        }

        for(u32 i = 0; i < ACNLTitleIDs.size(); i++) {
            u64 ID = ACNLTitleIDs.at(i);
            if (std::find(titles.begin(), titles.end(), ID) != titles.end()) {
                InstalledTitles.SD_Titles |= (1 << i); //Set ACNL_Game values
            }
        }
        titles.clear();
    }
    return res;
}

FS::ACNL_TitlesInstalled FS::GetInstalledTitles(void) {
    SetACNLTitlesOnCart(); //Set the ACNL on cart again incase changed
    return InstalledTitles;
}

/*
    bool IsSDCardInserted(void)
        => returns whether or not the SD Card is inserted
*/
bool FS::IsSDCardInserted(void) {
    bool inserted = false;
    FSUSER_IsSdmcDetected(&inserted);
    return inserted;
}

/*
    bool IsGameCartInserted(void)
        => returns whether or not a GameCard is inserted
*/
bool FS::IsGameCartInserted(void) {
    bool inserted = false;
    FSUSER_CardSlotIsInserted(&inserted);
    return inserted;
}

/*
    bool checkGameCartTitleSame(void)
        => Checks if the currently inserted game cart is the same type as the one being edited.
            => Returns bool: IsCardSame
*/
bool checkGameCartTitleSame(u64 titleId) {
    if (currentMediaType != MEDIATYPE_GAME_CARD)
        return true; // We don't need to check if we aren't editing the game cart save file

    if (!FS::IsGameCartInserted()) return false;

    u32 num_gameCartTitles = 0;

    AM_GetTitleCount(MEDIATYPE_GAME_CARD, &num_gameCartTitles);
    u64 *gameCartTitles = new u64[num_gameCartTitles];
    AM_GetTitleList(NULL, MEDIATYPE_GAME_CARD, num_gameCartTitles, gameCartTitles);

    for (u32 i = 0; i < num_gameCartTitles; i++) {
        if (gameCartTitles[i] == titleId) {
            delete[] gameCartTitles;
            return true;
        }
    }

    delete[] gameCartTitles;
    return false;
}

void saveBackup(u64 tid)
{
    std::string path;
    char timestamp[100] = {0};
    time_t     rawtime;
    struct tm *timeinfo;

    path += Format(WORKDIR "/Saves/%016llX/", tid);
    FS::CreateDir(path);

    // Get timestamp for filename
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    /* String Format: DD-Month-YYYY_Hour(12h)-Minute-Secondam/pm */
    strftime(timestamp, 99, "[%d-%b-%Y_%I-%M-%S%p]", timeinfo);

    path += "garden_plus";
    path += timestamp;
    path += ".dat";
    if (!File::Exists(path))
    {
        File save(path, File::RWC|File::SYNC);
        save.Write(Save::Instance()->GetRawSaveData(), Save::Instance()->GetSaveSize());
        save.Close();
    }
    else
        MsgDisp(top, "Error:\nSave backup somehow already exists!");
}