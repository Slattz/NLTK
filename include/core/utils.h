#pragma once

#ifndef UTILS_H
#define UTILS_H

bool            is_ACNL(u64 tid);
bool            DebugCode(bool complete);
void            Sleep(s64 nanoseconds);
s64             Seconds(float amount);
s64             MicroSeconds(float amount);
u32             UTF8_StringSize(std::string str);
void            UTF8_String_PopBack(std::string& str);
std::string     u16tou8(std::u16string src);
std::u16string  u8tou16(const char* src);
bool            checkGameCartIsACNL();
std::string     Format(const char* fmt, ...);
u16             strToU16(std::string str);
void            LoadVillagerDatabase();
C2D_Image       ImageDataToC2DImage(u32 *imageData, u32 width, u32 height, GPU_TEXCOLOR colorFormat = GPU_TEXCOLOR::GPU_RGBA8);
void            C2D_ImageDelete(C2D_Image image);
bool            Utils_IsNew3DS(void);

#endif