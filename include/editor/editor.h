#pragma once

#ifndef EDITOR_H
#define EDITOR_H

#include <vector>
#include "gui/MainMenu.hpp"
#include "gui/PlayerMenu.hpp"
#include "menus.h"

namespace Editor
{
    static void (*DrawFunction)(void);
    static std::vector<void (*)(void)> DrawFunctionList;

    void Cleanup(void);
    void Init(void);
    ReturnMode Main(void);
    void Draw(void);

    void (*PreviousDrawFunction(void))(void);
    void SetDrawFunction(void (*)());
}

#endif