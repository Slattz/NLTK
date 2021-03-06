#include <3ds.h>
#include <string>
#include <map>
#include <fstream>
#include "item.h"
#include "save.h"
#include "utils.h"
#include "gfx.h"

//Real Max Item ID is 0x372B (excludes wrap paper) but for checks, ID -= 0x2000 from orig item ID;
static constexpr u16 maxID = 0x172B;

//This table lists the icons in the same order as the game, except it has been adapted for our spritesheet instead
static constexpr s32 SpritesheetIDTable[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 234, 235, 236, 225, 226, 227, 228, 229, 230, 231, 232, 233, 237, 238, 239, 241, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 1, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 266, 379, 339, 242, 240, 393 };

File *g_ItemBin = nullptr;
File *g_ItemKindBin = nullptr;
std::map<u16, std::string> g_itemDatabase;
//std::map<std::string, std::map<u16, std::string>> g_sortedItemDatabase;

void Item::LoadDatabase(void) {
    g_itemDatabase.clear();
    //g_sortedItemDatabase.clear();

    std::string currentLine;
    std::ifstream itemDatabase("romfs:/Items_en.txt", std::ifstream::in);
    std::string itemIdStr;
    std::string itemName;
    std::string sectionName = "None";

    while (std::getline(itemDatabase, currentLine))
    {
        if (currentLine.find("//") == 0)
        {
            sectionName = currentLine.substr(2);
        }
        else if (currentLine.size() > 8 && currentLine.find("//") == std::string::npos)
        {                                         // confirm we don't have any comments
            itemIdStr = currentLine.substr(2, 4); // skip the 0x hex specifier
            itemName = currentLine.substr(8, currentLine.size() - 9);

            // Convert itemIdStr to a u16
            u16 itemId = strToU16(itemIdStr);

            // Add item to the database
            g_itemDatabase.insert(std::make_pair(itemId, itemName));
            //g_sortedItemDatabase[sectionName][itemId] = itemName;
        }
    }

    itemDatabase.close();
}

void Item::LoadItemBins(void) {
    Result res = 0;
    g_ItemBin = new File();
    res = File::Open(*g_ItemBin, "romfs:/ItemBins/Item.bin", File::RB);
    if (R_FAILED(res)) {
        MsgDisp(top, Format("Failed opening Item.bin:\nResult: %X", res));
        if (g_ItemBin != nullptr) delete g_ItemBin;
        g_ItemBin = nullptr;
    }

    g_ItemKindBin = new File();
    res = File::Open(*g_ItemKindBin, "romfs:/ItemBins/Kind.bin", File::RB);
    if (R_FAILED(res)) {
        MsgDisp(top, Format("Failed opening Kind.bin:\nResult: %X", res));
        if (g_ItemKindBin != nullptr) delete g_ItemKindBin;
        g_ItemKindBin = nullptr;
    }
}

void Item::UnloadItemBins(void) {
    if (g_ItemBin != nullptr) {
        g_ItemBin->Close();
        g_ItemBin = nullptr;
    }
    if (g_ItemKindBin != nullptr) {
        g_ItemKindBin->Close();
        g_ItemKindBin = nullptr;
    }
}

File* Item::GetItemBin(void) {
    return g_ItemBin;
}

File* Item::GetItemKindBin(void) {
    return g_ItemKindBin;
}

Item::Item(const u16 id, const u16 flags) : ID(id), Flags(flags) { 
    Raw = (flags << 16) | id;
}

Item::Item(void) : Item(0x7FFE, 0) { }

Item::Item(const u32 offset)
    : Item(Save::Instance()->ReadU16(offset), Save::Instance()->ReadU16(offset + 2)) { }

std::string Item::GetName(void) {
    for (auto const& entry : g_itemDatabase) {
        if (entry.first == this->ID) {
            return entry.second;
        }
    }
    return std::string("???");
}

s32 Item::IsNormalItem(void) {
    u16 ID = this->ID & 0x7FFF;
    s32 chk = ID - 0x2000; //To cover items lower than 0x2000 (Enviroment Items)

    if (chk < maxID)
    return chk;

    else return -1;
}

ItemBin_s* Item::GetItemBinSlot(void) {
    ItemBin_s *ItemSlot = new ItemBin_s;
    s32 chk = this->IsNormalItem();

    if (chk < 0 || chk > maxID) {
        delete ItemSlot;
        return nullptr;
    }

    /* chk <= maxID */
    else if (g_ItemBin != nullptr) {
        g_ItemBin->Seek(sizeof(ItemBin_s)*chk, File::SET);
        if (R_SUCCEEDED(g_ItemBin->Read(reinterpret_cast<void*>(ItemSlot), sizeof(ItemBin_s))))
        return ItemSlot;
    }

    delete ItemSlot;
    return nullptr;
}

ItemKind_s* Item::GetItemKindSlot(void) {
    ItemKind_s *KindSlot = new ItemKind_s;
    ItemBin_s *ItemSlot = this->GetItemBinSlot();

    if (ItemSlot == nullptr) {
        delete KindSlot;
        return nullptr;
    }

    u8 ItemCategory = ItemSlot->Category;
    delete ItemSlot;

    if (ItemCategory >= 0x9B) {
        delete KindSlot;
        return nullptr;
    }

    else if (g_ItemKindBin != nullptr) {
        g_ItemKindBin->Seek(sizeof(ItemKind_s) * ItemCategory, File::SET);
        if (R_SUCCEEDED(g_ItemKindBin->Read(reinterpret_cast<void *>(KindSlot), sizeof(ItemKind_s))))
            return KindSlot;
    }

    delete KindSlot;
    return nullptr;
}

u16 Item::GetAxeDamageValue(void) {
    /* Used for Normal Axe and Silver Axe */
    if (this->ID != 0x334D && this->ID != 0x334E)
        return 0;

    return (this->Flags & 0xF);
}

u16 Item::GetAxeDamageIcon(u16 ItemIcon) {
    static constexpr u8 AxeDamageTable[] = {0, 0, 0, 1, 1, 1, 2, 2};
    u16 DamageValue = GetAxeDamageValue();
    if (DamageValue >= 8)
        return ItemIcon;

    return ItemIcon + AxeDamageTable[DamageValue];
}

bool Item::IsInvWhitelisted(void) {
    u16 ItemID = this->ID;
    ItemID &= 0x6000;
    if (ItemID == 0x4000) //If item is wrapping paper / Any item ID between 0x4000 <-> 0x5FFF
        return true;

    ItemKind_s *KindSlot = this->GetItemKindSlot();
    if (KindSlot == nullptr) return false;

    u32 Unk = !(KindSlot->Unknown2&0x10);
    if (Unk == static_cast<u32>(-1)) return true;

    return false;
}

bool Item::IsInvWhitelisted(u16 ItemID) {
    Item item(ItemID, 0);
    ItemID &= 0x6000;
    if (ItemID == 0x4000) //If item is wrapping paper / Any item ID between 0x4000 <-> 0x5FFF
        return true;

    ItemKind_s *KindSlot = item.GetItemKindSlot();
    if (KindSlot == nullptr) return false;

    u32 Unk = !(KindSlot->Unknown2&0x10);
    if (Unk == static_cast<u32>(-1)) return true;

    return false;
}

u8 Item::GetCategory(void) {
    ItemBin_s* ItemSlot = this->GetItemBinSlot();
    if (ItemSlot == nullptr)
        return 0x9B;

    u8 category = ItemSlot->Category;
    delete ItemSlot;
    if (category >= 0x9B)
        return 0;

    return category;
}

u16 Item::GetIconID(void) {

    ItemBin_s* ItemSlot = this->GetItemBinSlot();
    if (ItemSlot == nullptr)
        return 0;

    u16 IconID = ItemSlot->ItemIcon;
    delete ItemSlot;

    if (IconID >= 0x1FB) {
        return 0;
    }

    u8 category = this->GetCategory();
    if (category == 0xE && (this->ID == 0x334D || this->ID == 0x334E)) {
        IconID = GetAxeDamageIcon(IconID);
    }

    return IconID;
}

s32 Item::GetSpritesheetID(void) {
    if (this->ID == 0x7FFE) {
        return -1;
    }

    u16 iconID = 0;
    if (g_ItemBin != nullptr) {
        iconID = this->GetIconID();
    }

    return SpritesheetIDTable[iconID];
}
