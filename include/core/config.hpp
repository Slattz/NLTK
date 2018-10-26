#pragma once

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "json.hpp"

class Config : public Json {

  public:
    Config(const char* path);
    Config(void);
    ~Config(void);
    int Save(void);
    bool Reset(void);


    bool IsDebug = false;
    bool Auto_Update = false;
    bool Use_Sys_Keyboard = false;
    bool Auto_SaveBackup = false;
    int  Auto_loadprefMode = 0;
    bool Auto_loadprefGame = false;
    int  prefGame = 0;
    int  prefGameMediaType = 0;

  protected:
    void SetupValues(void);
    void ApplyValues(void);
    const char *m_configPath = nullptr;
};

#endif