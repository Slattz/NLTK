#include <3ds.h>
#include <cstring>
#include "json.hpp"

/* Decode a JSON from file */
Json::Json(const char* path) {
    m_json = json_load_file(path, 0, m_jsonError);
    m_jsonInit = (!m_json ? false : true);

    if (m_json && m_jsonInit)
    {
        m_size = json_object_size(m_json);
    }
}

/* Decode a JSON string buffer */
Json::Json(char *buf, size_t bufsize) {
    m_json = json_loadb(buf, bufsize, 0, m_jsonError);
    m_jsonInit = (!m_json ? false : true);

    if (m_json && m_jsonInit)
    {
        m_size = json_object_size(m_json); //Get how many keys in the json object
    }
}

/* Construct Json from json_t */
Json::Json(json_t *json) {
    m_json = json;
    m_jsonInit = (!m_json ? false : true);

    if (m_json && m_jsonInit)
    {
        m_size = json_object_size(m_json); //Get how many keys in the json object
    }
}

Json::Json(void) {
    m_jsonInit = false;
}

Json::~Json(void)
{
    while (m_json->refcount > 0)
    {
        json_decref(m_json);
    }
}

int Json::GetValue(const char *key, char *val, size_t valsize)
{
    json_t *jsn = json_object_get(m_json, key);

    if (jsn == NULL || !json_is_string(jsn))
        return -1;

    const char *string = json_string_value(jsn);
    strncpy(val, string, valsize);

    return 0;
}

int Json::GetValue(const char *key, bool *val)
{
    json_t *jsn = json_object_get(m_json, key);

    if (jsn == NULL || !json_is_boolean(jsn))
        return -1;

    *val = json_boolean_value(jsn);

    return 0;
}

int Json::GetValue(const char *key, int *val)
{
    json_t *jsn = json_object_get(m_json, key);

    if (jsn == NULL)
        return -1;

    *val = json_integer_value(jsn);
    return 0;
}

int Json::GetValue(const char *key, u64 *val)
{
    json_t *jsn = json_object_get(m_json, key);

    if (jsn == NULL)
        return -1;

    *val = json_integer_value(jsn);
    return 0;
}

int Json::GetValue(const char *key, u32 *val)
{
    json_t *jsn = json_object_get(m_json, key);

    if (jsn == NULL)
        return -1;

    *val = (json_integer_value(jsn) & 0xFFFFFFFF);
    return 0;
}

int Json::GetValue(const char *key, double *val)
{
    json_t *jsn = json_object_get(m_json, key);

    if (jsn == NULL)
        return -1;

    *val = json_real_value(jsn);
    return 0;
}

int Json::SetValue(const char *key, char *val)
{
    return json_object_set(m_json, key, json_pack("s", val));
}

int Json::SetValue(const char *key, bool val)
{
    return json_object_set(m_json, key, json_pack("b", val));
}

int Json::SetValue(const char *key, int val)
{
    return json_object_set(m_json, key, json_pack("i", val));
}

int Json::SetValue(const char *key, u64 val)
{
    return json_object_set(m_json, key, json_pack("o", val));
}

int Json::SetValue(const char *key, u32 val)
{
    return json_object_set(m_json, key, json_pack("o", val));
}

int Json::SetValue(const char *key, double val)
{
    return json_object_set(m_json, key, json_pack("f", val));
}
