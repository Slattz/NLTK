#pragma once

#ifndef SAVE
#define SAVE

#include "common.h"

class Player;

class Save {
public:
	Save();
	~Save();
	Save(FS_Archive archive, Handle *handle);

	u8 ReadU8(u32 offset);
	u16 ReadU16(u32 offset);
	u32 ReadU32(u32 offset);
	u64 ReadU64(u32 offset);
	void ReadArray(u8 *dst, u32 offset, u32 count);
	void ReadArrayU16(u16 *dst, u32 offset, u32 count);
	std::u16string ReadString(u32 offset, u32 maxSize);

	u8* GetRawSaveData(void);
	u64 GetSaveSize(void);

	u8 DeriveRegionLockID(u8 RegionID, u8 LanguageID);
	void UpdateSaveRegion(void);
	void FixSaveRegion(void);

	bool Write(u32 offset, u8 *buffer, u32 count);
	bool Write(u32 offset, u8 data);
	bool Write(u32 offset, u16 data);
	bool Write(u32 offset, u32 data);
	bool Write(u32 offset, u64 data);
	bool Write(u32 offset, std::u16string data, u32 maxSize);

	bool ChangesMade(void);
	void SetChangesMade(bool changesMade);

	bool Commit(bool close);
	void Close(void);

	Player *players = nullptr;
	Region_Lock RegionLock;

private:
	u8 *m_saveBuffer = nullptr;
	u64 m_saveSize;
	FS_Archive m_archive;
	Handle *m_handle = nullptr;
	bool m_changesMade;
};

#endif