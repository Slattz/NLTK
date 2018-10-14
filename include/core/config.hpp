#pragma once

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <jansson.h>

class Config {

public:
    Config(const char* path);
    Config(void);
    ~Config(void);
    int Save(void);
    
    //Get Various Data Types
    int  GetValue(const char *key, char *val, size_t valsize);
    int  GetValue(const char* key, bool* val);
    int  GetValue(const char* key, int* val);
    int  GetValue(const char* key, u64* val);
    int  GetValue(const char* key, u32* val);
    int  GetValue(const char* key, double* val);

    //Set Various Data Types
    int  SetValue(const char* key, char* val);
    int  SetValue(const char* key, bool val);
    int  SetValue(const char* key, int val);
    int  SetValue(const char* key, double val);

    json_t *m_config = nullptr;
    size_t m_configSize = 0; //How many keys in the config object
    json_error_t *m_jsonError = nullptr;
    const char *m_configPath = nullptr;
    bool m_configInit;
};

#endif