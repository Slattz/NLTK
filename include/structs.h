#pragma once

#ifndef STRUCTS_H
#define STRUCTS_H

#include <3ds.h>
#include <string>

enum GameType {
	NL_JPN,
	NL_USA,
	NL_EUR,
	NL_KOR,
	WA_JPN,
	WA_USA,
	WA_EUR,
	WA_KOR
};

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

//NLTK ACNL Titles Installed struct
struct NLTK_ACNLs_Installed
{
    bool ORIG_installed = false;
    bool WA_installed = false;
    bool ORIG_JPN_installed = false;
    bool ORIG_USA_installed = false;
    bool ORIG_EUR_installed = false;
    bool ORIG_KOR_installed = false;
    bool WA_JPN_installed = false;
    bool WA_USA_installed = false;
    bool WA_EUR_installed = false;
    bool WA_KOR_installed = false;

};

struct NLTK_Media_Installed {
	bool HasACNLData = false;
	NLTK_ACNLs_Installed InstalledTitles;
};

struct NLTK_Titles_Info {
	NLTK_Media_Installed GameCartInfo;
	NLTK_Media_Installed SDCardInfo;
};

struct Point_t {
	u32 X, Y;
};

struct Size_t {
	u32 Width, Height;
};

#endif