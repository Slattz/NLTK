#include <3ds.h>
#include <string>
#include "e_utils.h"
#include "item.h"

Item::Item() {
	Id = 0x7FFE;
	Flag1 = 0;
	Flag2 = 0;
	Name = std::string("Empty");
}

Item::Item(u16 id, u8 flag1, u8 flag2) {
	Id = id;
	Flag1 = flag1;
	Flag2 = flag2;
    Name = GetItemName(this);
	Icon = GetItemIcon(this);
}