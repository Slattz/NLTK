#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "gfx.h"
#include "pattern.h"
#include "jpeg.h"
#include "utils.h"
#include "player.h"

Player::Player() { }

Player::~Player()
{
    if (m_TPCPic.tex != nullptr) {
        C2D_ImageDelete(this->m_TPCPic);
        m_TPCPic.tex = nullptr;
        m_TPCPic.subtex = nullptr;
    }
    if (this->m_TPCData != nullptr)
        delete[] this->m_TPCData;
}

Player::Player(u32 offset, u32 index) {
    this->m_offset = offset;
    this->m_index = index;

    this->PlayerId = Save::Instance()->ReadU16(offset + 0x55A6);
    this->Name = Save::Instance()->ReadString(offset + 0x55A8, 8);
    this->Gender = Save::Instance()->ReadU16(offset + 0x55BA);
    this->TownId = Save::Instance()->ReadU16(offset + 0x55BC);
    this->TownName = Save::Instance()->ReadString(offset + 0x55BE, 8);

    this->Pockets = new Item[16];

    for (int i = 0; i < 16; i++) {
        this->Pockets[i] = Item(offset + 0x6BD0 + i * 4);
    }

    this->Savings = EncryptedInt32(Save::Instance()->ReadU64(offset + 0x6B8C));
    this->Debt = EncryptedInt32(Save::Instance()->ReadU64(offset + 0x6B94));
    this->IslandMedals = EncryptedInt32(Save::Instance()->ReadU64(offset + 0x6B9C));
    this->Wallet = EncryptedInt32(Save::Instance()->ReadU64(offset + 0x6F08));
    this->MeowCoupons = EncryptedInt32(Save::Instance()->ReadU64(offset + 0x8D1C));

    if (this->Exists()) {
        this->RefreshTPC();
    }

    for (u32 i = 0; i < 10; i++) {
        this->Patterns[i] = new Pattern(this, i);
    }
}

void Player::Write() {
    u32 encryptedInt = 0;
    u32 encryptionData = 0;

    // Always disable reset flag. TODO: Do we want this? Or should we make it a checkbox?
    this->SetHasReset(false);

    // TODO: Write Patterns

    Save::Instance()->Write(this->m_offset + 0x55A6, this->PlayerId);
    Save::Instance()->Write(this->m_offset + 0x55A8, this->Name, 8);
    Save::Instance()->Write(this->m_offset + 0x55BA, this->Gender);
    Save::Instance()->Write(this->m_offset + 0x55BC, this->TownId);
    Save::Instance()->Write(this->m_offset + 0x55BE, this->TownName, 8);

    this->Savings.encrypt(encryptedInt, encryptionData);
    Save::Instance()->Write(this->m_offset + 0x6B8C, encryptedInt);
    Save::Instance()->Write(this->m_offset + 0x6B90, encryptionData);

    this->Debt.encrypt(encryptedInt, encryptionData);
    Save::Instance()->Write(this->m_offset + 0x6B94, encryptedInt);
    Save::Instance()->Write(this->m_offset + 0x6B98, encryptionData);

    this->IslandMedals.encrypt(encryptedInt, encryptionData);
    Save::Instance()->Write(this->m_offset + 0x6B9C, encryptedInt);
    Save::Instance()->Write(this->m_offset + 0x6BA0, encryptionData);

    this->Wallet.encrypt(encryptedInt, encryptionData);
    Save::Instance()->Write(this->m_offset + 0x6F08, encryptedInt);
    Save::Instance()->Write(this->m_offset + 0x6F0C, encryptionData);

    this->MeowCoupons.encrypt(encryptedInt, encryptionData);
    Save::Instance()->Write(this->m_offset + 0x8D1C, encryptedInt);
    Save::Instance()->Write(this->m_offset + 0x8D20, encryptionData);
}

u8* Player::RefreshTPC() {

    if (m_TPCPic.tex != nullptr) {
        C2D_ImageDelete(this->m_TPCPic);
        m_TPCPic.tex = nullptr;
        m_TPCPic.subtex = nullptr;
    }

    if (Save::Instance()->ReadU16(this->m_offset + 0x5738) == 0xD8FF) { // 0xFFD8 = JPEG File Marker
        if (this->m_TPCData == nullptr)
            this->m_TPCData = new u8[0x1400];
        Save::Instance()->ReadArray(this->m_TPCData, this->m_offset + 0x5738, 0x1400);
        this->m_TPCPic = LoadPlayerPicture(this->m_TPCData);
    }
    else { //No TPC
        if (this->m_TPCData != nullptr)
        {
            delete[] this->m_TPCData;
            this->m_TPCData = nullptr;
        }
        this->m_TPCPic = C2D_SpriteSheetGetImage(Players_ss, NO_TPC_PIC);
    }

    return this->m_TPCData;
}

bool Player::Exists() {
    return Save::Instance()->ReadU16(this->m_offset + 0x55A6) != 0;
}

bool Player::HasReset() {
    return Save::Instance()->ReadU8(this->m_offset + 0x570A) & 2;
}

void Player::SetHasReset(bool reset) {
    u8 currentFlags = Save::Instance()->ReadU8(this->m_offset + 0x570A);
    if (reset) {
        Save::Instance()->Write(this->m_offset + 0x570A, static_cast<u8>(currentFlags | 2));
    }
    else {
        Save::Instance()->Write(this->m_offset + 0x570A, static_cast<u8>(currentFlags & ~2));
    }
}