#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "gfx.h"
#include "item.h"
#include "pattern.h"
#include "jpeg.h"
#include "player.h"

Player::Player() { }

Player::~Player()
{
    if (m_TPCData != nullptr)
        delete[] m_TPCData;
}

Player::Player(u32 offset, u32 index) {
    m_offset = offset;
    m_index = index;

    PlayerId = Save::Instance()->ReadU16(offset + 0x55A6);
    Name = Save::Instance()->ReadString(offset + 0x55A8, 8);
    Gender = Save::Instance()->ReadU16(offset + 0x55BA);
    TownId = Save::Instance()->ReadU16(offset + 0x55BC);
    TownName = Save::Instance()->ReadString(offset + 0x55BE, 8);

    Pockets = new Item[16];

    for (int i = 0; i < 16; i++) {
        u32 itemOffset = offset + 0x6BD0 + i * 4;
        Pockets[i] = Item(Save::Instance()->ReadU16(itemOffset), Save::Instance()->ReadU8(itemOffset + 2), Save::Instance()->ReadU8(itemOffset + 3));
    }

    Savings = EncryptedInt32(Save::Instance()->ReadU64(offset + 0x6B8C));
    Debt = EncryptedInt32(Save::Instance()->ReadU64(offset + 0x6B94));
    IslandMedals = EncryptedInt32(Save::Instance()->ReadU64(offset + 0x6B9C));
    Wallet = EncryptedInt32(Save::Instance()->ReadU64(offset + 0x6F08));
    MeowCoupons = EncryptedInt32(Save::Instance()->ReadU64(offset + 0x8D1C));

    if (Exists()) {
        RefreshTPC();
    }

    for (u32 i = 0; i < 10; i++) {
        Patterns[i] = new Pattern(this, i);
    }
}

void Player::Write() {
    u32 encryptedInt = 0;
    u32 encryptionData = 0;

    // TODO: Write Patterns

    Save::Instance()->Write(m_offset + 0x55A6, PlayerId);
    Save::Instance()->Write(m_offset + 0x55A8, Name, 8);
    Save::Instance()->Write(m_offset + 0x55BA, Gender);
    Save::Instance()->Write(m_offset + 0x55BC, TownId);
    Save::Instance()->Write(m_offset + 0x55BE, TownName, 8);

    Savings.encrypt(encryptedInt, encryptionData);
    Save::Instance()->Write(m_offset + 0x6B8C, encryptedInt);
    Save::Instance()->Write(m_offset + 0x6B90, encryptionData);

    Debt.encrypt(encryptedInt, encryptionData);
    Save::Instance()->Write(m_offset + 0x6B94, encryptedInt);
    Save::Instance()->Write(m_offset + 0x6B98, encryptionData);

    IslandMedals.encrypt(encryptedInt, encryptionData);
    Save::Instance()->Write(m_offset + 0x6B9C, encryptedInt);
    Save::Instance()->Write(m_offset + 0x6BA0, encryptionData);

    Wallet.encrypt(encryptedInt, encryptionData);
    Save::Instance()->Write(m_offset + 0x6F08, encryptedInt);
    Save::Instance()->Write(m_offset + 0x6F0C, encryptionData);

    MeowCoupons.encrypt(encryptedInt, encryptionData);
    Save::Instance()->Write(m_offset + 0x8D1C, encryptedInt);
    Save::Instance()->Write(m_offset + 0x8D20, encryptionData);
}

u8* Player::RefreshTPC() {

    if (Save::Instance()->ReadU16(m_offset + 0x5738) == 0xD8FF) { // 0xFFD8 = JPEG File Marker
        if (m_TPCData == nullptr)
            m_TPCData = new u8[0x1400];
        Save::Instance()->ReadArray(m_TPCData, m_offset + 0x5738, 0x1400);
        m_TPCPic = LoadPlayerPicture(m_TPCData);
    }
    else { //No TPC
        if (m_TPCData != nullptr)
        {
            delete[] m_TPCData;
            m_TPCData = nullptr;
        }
        m_TPCPic = C2D_SpriteSheetGetImage(Players_ss, NO_TPC_PIC);
    }

    return m_TPCData;
}

bool Player::Exists() {
    s16 id = Save::Instance()->ReadS16(m_offset + 0x55A6);
    return (id != 0 && id != -1);
}