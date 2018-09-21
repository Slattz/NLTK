#pragma once

#ifndef ITEM_H
#define ITEM_H

class Item {
public:
    Item();
    Item(u16 id, u8 flag1, u8 flag2);

    u16 Id;
    u8 Flag1;
    u8 Flag2;
    std::string Name;
    s32 Icon;
};

#endif