#include <3ds.h>
#include <citro2d.h>
#include <codecvt>
#include <locale>
#include <memory>
#include <map>
#include "file.hpp"
#include "item.h"
#include "save.h"
#include "player.h"
#include "e_utils.h"

extern File *g_ItemBin;
extern std::map<u16, std::string> g_itemDatabase;

static const int	iconIdTable[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 234, 235, 236, 225, 226, 227, 228, 229, 230, 231, 232, 233, 237, 238, 239, 241, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 1, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 266, 379, 339, 242, 240, 393 };

std::vector<std::pair<std::string, s32>> load_player_invitems(int selectedplayer) {
    std::vector<std::pair<std::string, s32>> inventoryItemData;
    for (int num = 0; num < 16; num++) {
        Item* item = &Save::Instance()->players[selectedplayer]->Pockets[num];
        inventoryItemData.push_back(std::make_pair(GetItemName(item), GetItemIcon(item)));
    }

    return inventoryItemData;
}

bool CheckUnreadMail(int selectedplayer)
{
    /* STUB until player struct finished*/
    if (selectedplayer == 0) { // TODO: REMOVE THIS (It's here to stop compiler warnings)
        return true;
    }
    return true;
}

Item GetItemFromSave(u32 offset)
{
    return Item(offset);
}

int CheckItemValidity(Item *item)
{
    //Real Max ID is 0x372B (exclude wrap paper) but for checks game -= 0x2000 from orig item ID
    static const s16 maxID = 0x172B;
    int chk;

    u16 ID = item->Id & ~0x8000;
    chk = ID - 0x2000; //To cover items lower than 0x2000 (Enviroment Items)

    if (chk < maxID)
    return chk;

    else return -1;
}

u32 CheckIfValidItem(Item *item)
{
    static const u16 maxID = 0x172B;
    int chk = CheckItemValidity(item);

    if (chk < 0 || chk > maxID)
        return 0;

    else /* chk <= maxID */
    {
        chk = ((chk << 4) - chk) << 1;
        return chk;
    }
}

u16 GetAxeDamageValue(Item *item)
{
    /* Used for Normal Axe and Silver Axe */
    if (item->Id != 0x334D && item->Id != 0x334E)
        return 0;

    return (item->Flags & 0xF); // TODO: Confirm Flag2 is correct
}

u16 GetAxeDamageIcon(Item *item, u16 ItemIcon)
{
    static u8 axedamagetable[] = {0, 0, 0, 1, 1, 1, 2, 2};
    u16 DamageValue = GetAxeDamageValue(item);
    if (DamageValue >= 8)
        return ItemIcon;

    return ItemIcon + axedamagetable[DamageValue];
}

u8 GetItemCategory(Item *item)
{
    if (g_ItemBin == nullptr)
        return 0;

    u32 offset = CheckIfValidItem(item);
    if (offset == 0)
        return 0x9B;

    g_ItemBin->Seek(offset+0xA);
    u8 category = 0;
    g_ItemBin->Read(category);
    if (category >= 0x9B)
        return 0;

    return category;
}

u16 GetIconID(Item *item)
{
    if (g_ItemBin == nullptr)
        return 0;

    u32 offset = CheckIfValidItem(item);
    if (offset == 0)
        return 0;

    u16 iconID = 0;
    g_ItemBin->Seek(offset);
    g_ItemBin->Read(iconID);

    if (iconID >= 0x1FB) {
        return 0;
    }

    u8 category = GetItemCategory(item);
    if (category == 0xE && (item->Id == 0x334D || item->Id == 0x334E)) {
        iconID = GetAxeDamageIcon(item, iconID);
    }

    return iconID;
}

s32 GetItemIcon(Item *item)
{
    if (item->Id == 0x7FFE) {
        return -1;
    }

    u16 iconID = 0;
    if (g_ItemBin != nullptr) {
        iconID = GetIconID(item);
    }

    return iconIdTable[iconID];
}

std::string GetItemName(Item *item)
{
    for (auto const& entry : g_itemDatabase) {
        if (entry.first == item->Id) {
            return entry.second;
        }
    }

    return std::string("???");
}

std::vector<u32> findPlayerReferences(Player *player) {
    // Read u16 ID + Name
    u16 *dataArray = new u16[11];
    for (u32 i = 0; i < 11; i++) {
        dataArray[i] = Save::Instance()->ReadU16(player->m_offset + 0x55A6 + i * 2);
    }

    std::vector<u32> references = std::vector<u32>();
    u32 Size = Save::Instance()->GetSaveSize() - 22;
    for (u32 i = 0; i < Size; i += 2) { // subtract 22 so we don't read past the end of the file
        bool foundMatch = true;
        for (int x = 0; x < 11; x++) {
            if (Save::Instance()->ReadU16(i + x * 2) != dataArray[x]) {
                foundMatch = false;
                break;
            }
        }
        // add the offset to the vector
        if (foundMatch) {
            references.push_back(i);
            i += 20; // skip the rest of this data
        }
    }

    delete[] dataArray;
    return references;
}