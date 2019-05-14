#pragma once

#ifndef ITEM_H
#define ITEM_H

#include "file.hpp"

struct ItemBin_s {
    u16 ItemIcon;
    u16 ItemPrice;
    u16 Unknown0;
    u16 RefurbishID;
    u16 SoundID;
    u8  Category;
    s8  Unknown2;
    s8  Unknown3;
    s8  Unknown4;
    s8  PriceMultiplier;
    u8  Unknown5;
    s8  ClothingGroup;
    u8  CanRefurbish___;
    u8  DinosaurGroup;
    s8  ArtGroup;
    u8  Unknown11;
    u8  Unknown12;
    u8  Unknown13;
    u8  Unknown14;
    u8  Unknown15;
    u8  Unknown16;
    u8  Unknown17;
    u8  Unknown18;
    u8  Unknown19;
    u8  Unknown20;
};

struct ItemKind_s {
    u8 Unknown1;
    u8 Unknown2;
    u8 Unknown3;
};

class Item {
public:
    union {
        u32 Raw;
        struct {
            u16 ID;
            u16 Flags;
        };
    };

    static void LoadDatabase(void);
    static void LoadItemBins(void);
    static void UnloadItemBins(void);
    static File* GetItemBin(void);
    static File* GetItemKindBin(void);
    static bool IsInvWhitelisted(u16 ItemID);

    Item(void);
    Item(const u32 offset);
    Item(const u16 id, const u16 flags);

    s32 IsNormalItem(void);
    ItemBin_s* GetItemBinSlot(void);
    ItemKind_s* GetItemKindSlot(void);
    u16 GetAxeDamageValue(void);
    u16 GetAxeDamageIcon(u16 ItemIcon);

    std::string GetName(void);
    bool IsInvWhitelisted(void);
    u8 GetCategory(void);
    u16 GetIconID(void);
    s32 GetSpritesheetID(void);

};

#endif