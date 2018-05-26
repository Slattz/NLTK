#pragma once

#ifndef PLAYER
#define PLAYER

#include "common.h"

class Save;
class Item;

class Player {
public:
	Player(void);
	~Player(void);
	Player(Save *save, u32 offset, u32 index);

	u8 Gender;
	u16 PlayerId;
	std::u16string Name;
	Item *Pockets = nullptr;
	EncryptedInt32 Wallet;
	EncryptedInt32 Debt;
	EncryptedInt32 Savings;
	EncryptedInt32 MeowCoupons;
	EncryptedInt32 IslandMedals;

	void Write(Save *save);
	u8* RefreshTPC(Save *save);
	bool Exists(Save *save);

	u32 m_offset;
	u32 m_index;
	u8 *m_TPCData = nullptr;
	C2D_Image m_TPCPic;
};

#endif