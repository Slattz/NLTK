#pragma once

#ifndef ITEM_H
#define ITEM_H

union Item {
    u32 Raw;
    struct {
        u16 Id;
        u16 Flags;
    };

    Item(void);
    Item(const u32 offset);
    Item(const u16 id, const u16 flags);
};

#endif