#pragma once

#ifndef SWKBD_H
#define SWKBD_H

typedef struct
{
    int mode;
    bool isHex;
    int maxValue;
    int maxLength;
} SWKBD_s;

std::string getKeyboardInput(Text myText, const char *hintText, u32 maxLength, SwkbdType keyboardType);

#endif