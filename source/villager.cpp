#include "villager.h"

Villager::Villager() { }

Villager::~Villager() {
    // Checking if a pointer is null is not necessary for delete. It will verify the memory is valid before attempting to delete it.
    if (this->m_villagerData != nullptr) {
        delete this->m_villagerData;
    }
}

Villager::Villager(const u32 offset, const u32 index) {
    this->m_offset = offset;
    this->m_index = index;
    this->m_villagerData = new Villager_s;

    // Read villager data into struct
    Save::Instance()->ReadArray(reinterpret_cast<u8 *>(this->m_villagerData), offset, sizeof(Villager_s));
}

u32 Villager::GetOffset() const {
    return this->m_offset;
}

u32 Villager::GetIndex() const {
    return this->m_index;
}