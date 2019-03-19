#pragma once

#ifndef STRUCTS_H
#define STRUCTS_H

#include <3ds.h>
#include <string>

//NLTK Settings
struct NLTK_Settings
{
    bool    LoadedACNLFont : 1;
    bool    LoadedItemBin : 1;
    bool    LoadedItemIcons : 1;
    u8      NLTK_Mode;
};

//Item Icon Info
struct LoadItem_s
{
    int     X;
    int     Y;
    std::string  ItemName;
};

struct Point_t {
    u32 X, Y;
};

struct Size_t {
    u32 Width, Height;
};

struct Rect_t {
    Point_t TopLeft;
    Point_t BottomRight;
};

#endif