#include "save.h"
#include "checksum.h"

Save::Save() { }

Save::~Save() {
	/*if (players)
		delete[] players;
	if (m_saveBuffer)
		delete[] m_saveBuffer;*/
}

Save::Save(FS_Archive archive, Handle *handle, bool init) {
	m_archive = archive;
	m_handle = handle;
	m_saveSize = 0;
	FS_Path path = fsMakePath(PATH_ASCII, "/garden_plus.dat");

	FSUSER_OpenFile(handle, archive, path, FS_OPEN_READ | FS_OPEN_WRITE, 0);
	FSFILE_GetSize(*handle, &m_saveSize);
	m_saveBuffer = new u8[m_saveSize];

	FSFILE_Read(*handle, NULL, 0, m_saveBuffer, m_saveSize);

	m_changesMade = false;

    if (!init) {
        return;
    }

	// Load Players
	players = new Player[4];
	for (int i = 0; i < 4; i++) {
		u32 PlayerOffset = 0xA0 + (i * 0xA480);
		players[i] = Player(this, PlayerOffset, i);
	}

	RegionLock.RawByte = ReadU8(0x621CE);
    RegionLock.DerivedID = RegionLock.RawByte & 0xF;
    RegionLock.RegionID = static_cast<CFG_Region>(RegionLock.RawByte >> 4);
}

u8 Save::ReadU8(u32 offset) {
	return m_saveBuffer[offset];
}

u16 Save::ReadU16(u32 offset) {
	return *(u16 *)(m_saveBuffer + offset);
}

u32 Save::ReadU32(u32 offset) {
	return *(u32 *)(m_saveBuffer + offset);
}

u64 Save::ReadU64(u32 offset) {
	return *(u64 *)(m_saveBuffer + offset);
}

void Save::ReadArray(u8 *dst, u32 offset, u32 count) {
	u8 *src = m_saveBuffer + offset;
	memcpy(dst, src, count);
}

void Save::ReadArrayU16(u16 *dst, u32 offset, u32 count) {
	u16 *src = (u16 *)(m_saveBuffer + offset);
	memcpy(dst, src, count << 1);
}

std::u16string Save::ReadString(u32 offset, u32 maxSize) {
	return std::u16string(reinterpret_cast<char16_t *>(m_saveBuffer + offset), maxSize + 1);
}

u64 Save::GetSaveSize(void) {
	return m_saveSize;
}

u8* Save::GetRawSaveData(void) {
	return m_saveBuffer;
}

//https://3dbrew.org/wiki/Title_list/DLC#Region_IDs
u8 Save::DeriveRegionLockID(u8 RegionID, u8 LanguageID) {
    if (RegionID != 0) { //If not Japan
        if (RegionID == 1) { //If region is USA
            switch (LanguageID) {
                case 2: //If lang is French
                    return 3;
				case 5: //If lang is Spanish
					return 2;
				default: //If lang is English & other langs
					return 1;
            }
        }

        else if (RegionID == 2) { //If region is EUR
            switch (LanguageID) {
                case 2: //If lang is French
                    return 6;
				case 3: //If lang is German
					return 8;
				case 4: //If lang is Italian
					return 7;
				case 5: //If lang is Spanish
					return 5;
                default:
                    return 4;
            }
        }

        else if (RegionID == 5) { //If region is KOR
            return 9;
        }
    }
    return 0;
}

bool Save::UpdateSaveRegion(void) {
	u8 SystemLanguage = 0xC; //0xC is ACNL default value || max+1
    u8 SystemRegion = 7; //7 is ACNL default value || max+1
	u8 RegionByte;
    bool ret = false;
	CFGU_SecureInfoGetRegion(&SystemRegion);
	CFGU_GetSystemLanguage(&SystemLanguage);

	u8 DeriveID = DeriveRegionLockID(SystemRegion, SystemLanguage);
    RegionByte = (SystemRegion << 4) | (DeriveID & 0xF);

    if (RegionLock.RawByte != RegionByte) {
        ret = true;
        RegionLock.RegionID = static_cast<CFG_Region>(SystemRegion);
        RegionLock.DerivedID = DeriveID;
        RegionLock.RawByte = RegionByte;
    }

    return ret;
}

void Save::FixSaveRegion(void) {
	bool diff = UpdateSaveRegion();
    if (diff)
    {
        if (MsgDisp(top, "The region of the save you\nhave selected does not\nmatch your console.\nWould you like to fix it?", MsgTypeConfirm)) {
            Write(0x621CE, RegionLock.RawByte);
            m_changesMade = true;
        }
    }
}

void Save::FixInvalidBuildings(void) {
	bool asked = false;
	for (int i = 0; i < 58; i++) {
		u8 building = ReadU8(0x04be88+(i * 4)); //Get building ID
		if ((building >= 0x12 && building <= 0x4B) || building > 0xFC) {
			if (!asked) {
				asked = true;
				if (!MsgDisp(top, "Invalid building(s) have been\nfound in your save.\nThey may cause your game to crash.\nWould you like to fix it?", MsgTypeConfirm)) {
					return;
				}
			}

			Write(0x04be88+(i*4), static_cast<u32>(0x000000FC)); //Write empty building
			m_changesMade = true;
		}
	}
}

bool Save::Write(u32 offset, u8 *data, u32 count) {
	if (offset + count >= m_saveSize) {
    	return false;
	}

	memcpy(m_saveBuffer + offset, data, count);
	m_changesMade = true;
	return true;
}

bool Save::Write(u32 offset, u8 data) {
	return Write(offset, &data, 1);
}

bool Save::Write(u32 offset, u16 data) {
	return Write(offset, (u8 *)&data, 2);
}

bool Save::Write(u32 offset, u32 data) {
	return Write(offset, (u8 *)&data, 4);
}

bool Save::Write(u32 offset, u64 data) {
	return Write(offset, (u8 *)&data, 8);
}

bool Save::Write(u32 offset, std::u16string str, u32 maxSize) {
	if (str.length() > maxSize + 1) {
		return false;
	}

	return Write(offset, (u8 *)str.data(), maxSize * 2);
}

bool Save::ChangesMade(void) {
	return m_changesMade;
}

/*
	NOTE: This should be removed at some point. It's a hack to allow the Player encryptedInts to say they've been changed.
*/
void Save::SetChangesMade(bool changesMade) {
	m_changesMade = changesMade;
}

bool Save::Commit(bool close) {
	// Save Players
	for (int i = 0; i < 4; i++) {
		players[i].Write(this);
	}

	// Update Checksums
	FixCRC32s(this);

	bool res = R_SUCCEEDED(FSFILE_Write(*m_handle, NULL, 0, m_saveBuffer, m_saveSize, FS_WRITE_FLUSH));

	if (res) {
		m_changesMade = false;
	}

	if (close) {
		Close();
	}

	return res;
}

void Save::Close(void) {
	if (m_changesMade) {
		if (MsgDisp(top, "Would you like to save all changes\nmade to your save before exiting?", MsgTypeConfirm)) {
			Commit(false);
		}
	}

	if (R_SUCCEEDED(FSFILE_Close(*m_handle))) {
		FSUSER_ControlArchive(m_archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
	}

	FSUSER_CloseArchive(m_archive);
}