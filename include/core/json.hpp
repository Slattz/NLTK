#pragma once

#ifndef JSON_HPP
#define JSON_HPP

#include <jansson.h>

class Json {

public:
    Json(const char* path);
    Json(char *buf, size_t bufsize);
    Json(json_t *json);
    Json(void);
    ~Json(void);
    
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

    json_t *m_json = nullptr;
    size_t m_size = 0; //How many keys in the json object
    json_error_t *m_jsonError = nullptr;
    bool m_jsonInit = false;
};

#endif