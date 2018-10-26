#include <3ds.h>
#include <cstring>
#include "common.h"
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
        this->Save(); //Save (updated) config
    }
}

int Config::Save(void) {
    if (m_jsonInit) {
        return json_dump_file(m_json, m_configPath, JSON_INDENT(4));
    }
    return -2;
}