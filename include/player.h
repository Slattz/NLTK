#pragma once

#ifndef PLAYER_H
#define PLAYER_H

#include <citro2d.h>
#include <string>
#include "encryptedInt32.h"
#include "item.h"
#include "save.h"

class Pattern;

class Player {
public:
    Player(void);
    ~Player(void);
    Player(u32 offset, u32 index);

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

    void Write();
    u8* RefreshTPC();
    bool Exists();
    bool HasReset();
    void SetHasReset(bool reset);

    u32 m_offset;
    u32 m_index;
    bool m_HasTPC = false;
    u8 *m_TPCData = nullptr;
    C2D_Image m_TPCPic = {nullptr, nullptr};
};

#endif
