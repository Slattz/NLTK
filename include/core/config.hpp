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

    const char *m_configPath = nullptr;
};

#endif