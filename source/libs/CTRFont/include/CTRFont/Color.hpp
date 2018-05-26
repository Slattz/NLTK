#pragma once

#include <3ds/types.h>

struct Color
{
    Color(void);
    Color(u32 raw);
    Color(u8 r, u8 g, u8 b, u8 a = 255);

    union
    {
        u32 raw{ 0 };
        struct
        {
            u8  r;
            u8  g;
            u8  b;
            u8  a;
        } PACKED;
    };

    // Some basic colors constants
    static const Color  Black;
    static const Color  White;
    static const Color  Red;
    static const Color  Lime;
    static const Color  Blue;
    static const Color  Yellow;
    static const Color  Cyan;
    static const Color  Magenta;
    static const Color  Silver;
    static const Color  Gray;
    static const Color  Maroon;
    static const Color  Olive;
    static const Color  Green;
    static const Color  Purple;
    static const Color  Teal;
    static const Color  Navy;
};
