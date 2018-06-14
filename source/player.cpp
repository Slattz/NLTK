#include "player.h"
#include "pattern.h"

Player::Player() { }

Player::~Player()
{
	if (m_TPCData != nullptr)
		delete[] m_TPCData;
}

Player::Player(Save *save, u32 offset, u32 index) {
	m_offset = offset;
	m_index = index;

	PlayerId = save->ReadU16(offset + 0x55A6);
	Name = save->ReadString(offset + 0x55A8, 8);

	Pockets = new Item[16];

	for (int i = 0; i < 16; i++) {
		u32 itemOffset = offset + 0x6BD0 + i * 4;
		Pockets[i] = Item(save->ReadU16(itemOffset), save->ReadU8(itemOffset + 2), save->ReadU8(itemOffset + 3));
	}

	Savings = EncryptedInt32(save->ReadU64(offset + 0x6B8C));
	Debt = EncryptedInt32(save->ReadU64(offset + 0x6B94));
	IslandMedals = EncryptedInt32(save->ReadU64(offset + 0x6B9C));
	Wallet = EncryptedInt32(save->ReadU64(offset + 0x6F08));
	MeowCoupons = EncryptedInt32(save->ReadU64(offset + 0x8D1C));

	if (Exists(save)) {
		RefreshTPC(save);
	}

	for (u32 i = 0; i < 10; i++) {
		Patterns[i] = new Pattern(save, this, i);
	}
}

void Player::Write(Save *save) {
	u32 encryptedInt = 0;
	u32 encryptionData = 0;

	// TODO: Write Patterns

	save->Write(m_offset + 0x55A6, PlayerId);
	save->Write(m_offset + 0x55A8, Name, 8);

	Savings.encrypt(encryptedInt, encryptionData);
	save->Write(m_offset + 0x6B8C, encryptedInt);
	save->Write(m_offset + 0x6B90, encryptionData);

	Debt.encrypt(encryptedInt, encryptionData);
	save->Write(m_offset + 0x6B94, encryptedInt);
	save->Write(m_offset + 0x6B98, encryptionData);

	IslandMedals.encrypt(encryptedInt, encryptionData);
	save->Write(m_offset + 0x6B9C, encryptedInt);
	save->Write(m_offset + 0x6BA0, encryptionData);

	Wallet.encrypt(encryptedInt, encryptionData);
	save->Write(m_offset + 0x6F08, encryptedInt);
	save->Write(m_offset + 0x6F0C, encryptionData);

	MeowCoupons.encrypt(encryptedInt, encryptionData);
	save->Write(m_offset + 0x8D1C, encryptedInt);
	save->Write(m_offset + 0x8D20, encryptionData);
}

u8* Player::RefreshTPC(Save *save) {

	if (save->ReadU16(m_offset + 0x5738) == 0xD8FF) { // 0xFFD8 = JPEG File Marker
		if (m_TPCData == nullptr)
			m_TPCData = new u8[0x1400];
		save->ReadArray(m_TPCData, m_offset + 0x5738, 0x1400);
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

bool Player::Exists(Save *save) {
	u16 id = save->ReadU16(m_offset + 0x55A6);

	if (id == 0 || id == 0xFFFF) {
		return false;
	}

	return true;
}