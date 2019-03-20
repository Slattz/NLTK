#pragma once

#ifndef NFS_H
#define NFS_H

namespace FS {

    enum ACNL_Game : u8 {
        NONE = 0,
        ORIG_JPN = 1,
        ORIG_USA = 1 << 1,
        ORIG_EUR = 1 << 2,
        ORIG_KOR = 1 << 3,
        WA_JPN = 1 << 4,
        WA_USA = 1 << 5,
        WA_EUR = 1 << 6,
        WA_KOR = 1 << 7,

        
        ORIG_ANY = ORIG_JPN | ORIG_USA | ORIG_EUR | ORIG_KOR,
        WA_ANY = WA_JPN | WA_USA | WA_EUR | WA_KOR,
    };

    struct ACNL_TitlesInstalled {
        u8 SD_Titles = ACNL_Game::NONE;
        u8 Cart_Titles = ACNL_Game::NONE;
    };

    Result Initialize(void);
    void Cleanup(void);
    Result CreateDir(const std::string& path);
    FS_Archive GetSDMCArchive(void);
    Result OpenSaveArchive(FS_Archive *archive, FS_MediaType MediaType, u64 TitleID);
    bool IsSaveAccessible(FS_MediaType MediaType, u64 TitleID);

    ACNL_TitlesInstalled GetInstalledTitles(void);
    bool IsSDCardInserted(void);
    bool IsGameCartInserted(void);
}

int get_file_size(FILE *file);
bool fileExists(const char *path);
bool file_write(void *sourcebuf, const char *path, int size);
bool file_read(void* destbuf, const char *path, int size);
void saveBackup(u64 tid);
bool checkGameCartTitleSame(u64 titleId);

#endif