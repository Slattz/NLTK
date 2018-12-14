#include <3ds.h>
#include "save.h"
#include "item.h"

Item::Item(const u16 id, const u16 flags) : Id(id), Flags(flags) { 
    Raw = (flags << 16) | id;
}

Item::Item() : Item::Item(0x7FFE, 0) { }

Item::Item(const u32 offset)
    : Item::Item(Save::Instance()->ReadU16(offset), Save::Instance()->ReadU16(offset + 2)) { }