#pragma once

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "json.hpp"

class Config : public Json {

public:
    static Config* Instance(void);

    int Save(void);
    bool Reset(void);


    bool IsDebug = false;
    bool Auto_Update = false;
    bool Auto_SaveBackup = false;
    int  Auto_loadprefMode = 0;
    bool Auto_loadprefGame = false;
    int  prefGame = 0;
    int  prefGameMediaType = 0;

private:
    Config(void);
    ~Config(void);
    void SetupValues(void);
    void ApplyValues(void);

    static Config* m_Instance;
    const char *m_configPath = nullptr;
};

#endif