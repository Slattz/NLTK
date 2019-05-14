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

std::vector<std::pair<std::string, s32>> load_player_invitems(int selectedplayer) {
    std::vector<std::pair<std::string, s32>> inventoryItemData;
    for (int num = 0; num < 16; num++) {
        Item* item = &Save::Instance()->players[selectedplayer]->Pockets[num];
        inventoryItemData.push_back(std::make_pair(item->GetName(), item->GetSpritesheetID()));
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