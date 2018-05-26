#pragma once

#ifndef UTILS_H
#define UTILS_H

#include "common.h"

class Item;

bool IsSDCardInserted(void);
bool IsGameCartInserted(void);
int     RandomNum(int start, int end);
u32     DecryptACNLValue(u64 encval);
u64     EncryptACNLValue(u32 decval);
char    Sstrncpy(char *dest, const char *src, size_t n);
bool    is_ACNL(u64 tid);
bool    DebugCode(bool complete);
void    Sleep(u64 nanoseconds);
s64     Seconds(float amount);
void    load_player_invitems(Save *saveFile, int selectedplayer);
float   getCenterText_X(Text* msg, float StartX, float WidthX);
float   getCenterText_Y(Text* msg, float StartY, float HeightY);
void    draw_centered_text(float StartX, float WidthX, float StartY, float HeightY,
                        float scaleX, float scaleY, u32 color, const char* text);
void draw_centered_textf(float StartX, float WidthX, float StartY, float HeightY,
                        float scaleX, float scaleY, u32 color, const char* text, ...);
void  updatePointerPosition(void);
void  updateCursorInfo(void);
bool  CheckUnreadMail(int selectedplayer);
Item  GetItemFromSave(Save *saveFile, u32 offset);
int   CheckItemValidity(Item *item);
u32   CheckIfValidItem(Item *item);
u8    GetItemCategory(Item *item);
u16   GetIconID(Item *item, LoadItem_s *itemData);
std::string GetItemName(Item *item);
std::string u16tou8(std::u16string src);
std::u16string u8tou16(const char* src);
bool checkGameCartIsACNL();
std::string Format(const char* fmt, ...);


#endif