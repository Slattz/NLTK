#pragma once

#ifndef CONFIGMENU_HPP
#define CONFIGMENU_HPP

namespace Core
{
    void Spawn_ConfigMenu(void);
    void Draw_ConfigMenu(void);

    namespace ConfigMenu
    {
        void Initialize(void);
        void Cleanup(void);
    }
}

#endif