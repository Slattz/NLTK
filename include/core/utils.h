#pragma once

#ifndef UTILS_H
#define UTILS_H

bool            IsSDCardInserted(void);
bool            IsGameCartInserted(void);
bool            is_ACNL(u64 tid);
bool            DebugCode(bool complete);
void            Sleep(u64 nanoseconds);
s64             Seconds(float amount);
std::string     u16tou8(std::u16string src);
std::u16string  u8tou16(const char* src);
bool            checkGameCartIsACNL();
std::string     Format(const char* fmt, ...);
void            loadItemDatabase();
void            LoadVillagerDatabase();
C2D_Image       ImageDataToC2DImage(u32 *imageData, u32 width, u32 height, GPU_TEXCOLOR colorFormat = GPU_TEXCOLOR::GPU_RGBA8);
void            C2D_ImageDelete(C2D_Image image);

#endif