#pragma once

#ifndef PLAYER
#define PLAYER

#include "common.h"

class Save;
class Item;
class Pattern;

class Player {
public:
    Player(void);
    ~Player(void);
    Player(Save *save, u32 offset, u32 index);

    Pattern *Patterns[10];
    u16 PlayerId;
    std::u16string Name;
    u16 Gender; // might not be a u16, but the following byte is always? 0.
    u16 TownId;
    std::u16string TownName;
    Item *Pockets = nullptr;
    EncryptedInt32 Wallet;
    EncryptedInt32 Debt;
    EncryptedInt32 Savings;
    EncryptedInt32 MeowCoupons;
    EncryptedInt32 IslandMedals;

    void Write(Save *save);
    u8* RefreshTPC(Save *save);
    bool Exists(Save *save);

    u32 m_offset;
    u32 m_index;
    u8 *m_TPCData = nullptr;
    C2D_Image m_TPCPic;
};

#endif
