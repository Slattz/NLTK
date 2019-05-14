#pragma once

#ifndef E_UTILS_H
#define E_UTILS_H

#include <vector>
#include "save.h"
#include "item.h"

std::vector<std::pair<std::string, s32>> load_player_invitems(int selectedplayer);
bool  CheckUnreadMail(int selectedplayer);
u16   GetAxeDamageValue(Item *item);
u16   GetAxeDamageIcon(Item *item, u16 ItemIcon);
u8    GetItemCategory(Item *item);
u16   GetIconID(Item *item);
s32 GetItemIcon(Item *item);
std::string GetItemName(Item *item);
std::vector<u32> findPlayerReferences(Player *player);

#endif