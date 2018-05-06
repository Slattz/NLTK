#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_VERSIONMAJOR 1
#define CONFIG_VERSIONMINOR 2

typedef struct config_t
{
    char magic[4];
    u16  configVersionMajor, configVersionMinor;
    bool isdebug;
    bool autoupdate;
    bool autosavebackup;
    bool autoloadprefGame;
    u64  prefGame;
} NLTK_config;

void configInit(void);
void saveConfig(void);
void resetConfig(void);

#endif