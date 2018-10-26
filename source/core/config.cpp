#include <3ds.h>
#include <cstring>
#include "common.h"
#include "gfx.h"
#include "utils.h"
#include "config.hpp"

static const char* configPath = WORKDIR "/config.json";
static const char* configRomPath = "romfs:/config.json";

Config::Config(const char* path) : Json(path) {
    m_configPath = path;
}


/* Load Config From Default Path */
Config::Config(void) : Json(configRomPath) {
    m_configPath = configPath;

    if (m_json && m_jsonInit)
    {
        json_t *sd_config = json_load_file(configPath, 0, m_jsonError); //load config from sd
        if (sd_config) //If sd config existed
        {
            json_object_update_existing(m_json, sd_config); //update loaded romfs config json with the values from sd config
            json_decref(sd_config);
        }
        this->SetupValues(); //Setup the values
        this->Save(); //Save (updated) config

    }
}

Config::~Config() { }

void Config::SetupValues(void) {
    GetValue("debug", &IsDebug);
    GetValue("auto_update", &Auto_Update);
    GetValue("use_sys_keyboard", &Use_Sys_Keyboard);
    GetValue("auto_savebackup", &Auto_SaveBackup);
    GetValue("auto_loadprefMode", &Auto_loadprefMode);
    GetValue("auto_loadprefGame", &Auto_loadprefGame);
    GetValue("prefGame", &prefGame);
    GetValue("prefGameMediaType", &prefGameMediaType);
}

void Config::ApplyValues(void) {
    SetValue("debug", IsDebug);
    SetValue("auto_update", Auto_Update);
    SetValue("use_sys_keyboard", Use_Sys_Keyboard);
    SetValue("auto_savebackup", Auto_SaveBackup);
    SetValue("auto_loadprefMode", Auto_loadprefMode);
    SetValue("auto_loadprefGame", Auto_loadprefGame);
    SetValue("prefGame", prefGame);
    SetValue("prefGameMediaType", prefGameMediaType);
}

int Config::Save(void) {
    if (m_jsonInit) {
        ApplyValues();
        return json_dump_file(m_json, m_configPath, JSON_INDENT(4));
    }
    return -2;
}

bool Config::Reset(void) {
    if (m_jsonInit) {
        json_decref(m_json);
        m_json = nullptr;
        m_json = json_load_file(configRomPath, 0, m_jsonError); //load config from romfs as new config
        m_jsonInit = (!m_json ? false : true);
        this->SetupValues();
        this->Save();
    }

    return m_jsonInit;
}