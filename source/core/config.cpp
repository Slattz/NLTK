#include "common.h"

static const char* configPath = WORKDIR "/cfg.bin";
NLTK_config config;

void saveConfig(void)
{
    if (fileExists((char *)configPath))
        remove((char *)configPath);

    if (!file_write(&config, (char *)configPath, sizeof(NLTK_config)))
        infoDisp(top, "Error writing the configuration file");

}

void resetConfig(void)
{
    memcpy(&config.magic, "NLTK", 4);
    config.configVersionMajor = CONFIG_VERSIONMAJOR;
    config.configVersionMinor = CONFIG_VERSIONMINOR;
    config.isdebug = false;
    config.autoupdate = false;
    config.autosavebackup = false;
    config.autoloadprefGame = false;
    config.prefGame = 0;

    saveConfig();
}

bool loadConfigFromFile(void)
{
    if (!fileExists((char *)configPath))
        return false;

    file_read(&config, (char *)configPath, sizeof(NLTK_config));
    return true;
}

bool checkValidConfig(void)
{
    bool ret;

    if (sizeof(config) != sizeof(NLTK_config))
        ret = false;

    else if (memcmp(config.magic, "NLTK", 4) != 0)
        ret = false;

    else if (config.configVersionMajor != CONFIG_VERSIONMAJOR ||
            config.configVersionMinor != CONFIG_VERSIONMINOR)
        ret = false;

    else ret = true;

    if (!ret)
        memset(&config, 0, sizeof(NLTK_config));

    return ret;
}

void configInit(void)
{
    if (!loadConfigFromFile())
        resetConfig();

    if (!checkValidConfig())
        resetConfig();
}