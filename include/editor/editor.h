#pragma once

#ifndef EDITOR_H
#define EDITOR_H

#include "editor/e_gfx.h"
#include "editor/e_menus.h"
#include "gui/MainMenu.hpp"
#include "gui/PlayerMenu.hpp"

namespace Editor
{
    void Cleanup(void);
    void Init(void);
    int  Main(void);

}

#endif