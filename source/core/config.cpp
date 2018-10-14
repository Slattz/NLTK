#include <3ds.h>
#include <cstring>
#include "common.h"
#include "config.hpp"

static const char* configPath = WORKDIR "/config.json";
static const char* configRomPath = "romfs:/config.json";

Config::Config(const char* path) {
    m_configPath = path;
    m_config = json_load_file(configRomPath, 0, m_jsonError); //load default config from romfs
    m_configInit = (!m_config ? false : true);

    if (m_config && m_configInit)
    {
        m_configSize = json_object_size(m_config);
        json_t *sd_config = json_load_file(path, 0, m_jsonError); //load config from sd
        if (sd_config) //If sd config existed
        {
            json_object_update_existing(m_config, sd_config); //update the loaded romfs config with the values from sd config
            json_decref(sd_config);
        }
        this->Save(); //Save (updated) config
    }
}

Config::Config(void) : Config(configPath) {} //Default path config

Config::~Config(void) {
    while(m_config->refcount > 0) {
        json_decref(m_config);
    }
}

int Config::Save(void) {
    if (m_configInit) {
        return json_dump_file(m_config, m_configPath, JSON_INDENT(4));
    }
    return -2;
}

int Config::GetValue(const char* key, char* val, size_t valsize) {
    json_t* jsn = json_object_get(m_config, key);
    
    if (jsn == NULL) return -1;

    const char* string = json_string_value(jsn);
    strncpy(val, string, valsize);
    
    return 0;
}

int Config::GetValue(const char* key, bool* val) {
    json_t *jsn = json_object_get(m_config, key);

    if (jsn == NULL) return -1;

    *val = json_boolean_value(jsn);

    return 0;
}

int Config::GetValue(const char* key, int* val) {
    json_t *jsn = json_object_get(m_config, key);

    if (jsn == NULL) return -1;

    *val = json_integer_value(jsn);
    return 0;
}

int Config::GetValue(const char* key, u64* val) {
    json_t *jsn = json_object_get(m_config, key);

    if (jsn == NULL) return -1;

    *val = json_integer_value(jsn);
    return 0;
}

int Config::GetValue(const char* key, u32* val) {
    json_t *jsn = json_object_get(m_config, key);

    if (jsn == NULL) return -1;

    *val = (json_integer_value(jsn)&0xFFFFFFFF);
    return 0;
}

int Config::GetValue(const char* key, double* val) {
    json_t *jsn = json_object_get(m_config, key);

    if (jsn == NULL) return -1;

    *val = json_real_value(jsn);
    return 0;
}

int Config::SetValue(const char* key, char* val) {
    return json_object_set(m_config, key, json_pack("s", val));
}

int Config::SetValue(const char* key, bool val) {
    return json_object_set(m_config, key, json_pack("b", val));
}

int Config::SetValue(const char* key, int val) {
    return json_object_set(m_config, key, json_pack("i", val));
}

int Config::SetValue(const char* key, double val) {
    return json_object_set(m_config, key, json_pack("f", val));
}
