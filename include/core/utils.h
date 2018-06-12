#pragma once

#ifndef UTILS_H
#define UTILS_H

#include "common.h"

class Item;

bool IsSDCardInserted(void);
bool IsGameCartInserted(void);
char    Sstrncpy(char *dest, const char *src, size_t n);
bool    is_ACNL(u64 tid);
bool    DebugCode(bool complete);
void    Sleep(u64 nanoseconds);
s64     Seconds(float amount);
void    draw_centered_text(float StartX, float WidthX, float StartY, float HeightY,
                        float scaleX, float scaleY, u32 color, const char* text);
void  updatePointerPosition(void);
void  updateCursorInfo(void);
std::string u16tou8(std::u16string src);
std::u16string u8tou16(const char* src);
bool checkGameCartIsACNL();
std::string Format(const char* fmt, ...);
void loadItemDatabase();

#endif