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
	while (count > 0) {
		*dst++ = *src++;
		count--;
	}
}

void Save::ReadArrayU16(u16 *dst, u32 offset, u32 count) {
	u16 *src = (u16 *)(m_saveBuffer + offset);
	while (count > 0) {
		*dst++ = *src++;
		count--;
	}
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

u8 Save::DeriveRegionLockID(u8 RegionID, u8 LanguageID) {
    if (RegionID != 0) {
        if (RegionID == 1) {
            switch (LanguageID) {
                case 2:
                    return 3;
                case 5:
                    return 2;
                default:
                    return 1;
            }
        }

        else if (RegionID == 2) {
            switch (LanguageID) {
                case 2:
                    return 6;
                case 3:
                    return 8;
                case 4:
                    return 7;
                case 5:
                    return 5;
                default:
                    return 4;             
            }
        }

        else if (RegionID == 5) {
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
        if (confirmDisp(GFX_TOP, "The region of the save you\nhave selected does not\nmatch your console.\nWould you like to fix it?")) {
            Write(0x621CE, RegionLock.RawByte);
            m_changesMade = true;
        }
    }
}

bool Save::Write(u32 offset, u8 *data, u32 count) {
	if (offset >= m_saveSize || (offset + count) >= m_saveSize) {
		return false;
	}

	for (u32 i = 0; i < count; i++) {
		m_saveBuffer[offset + i] = data[i];
	}

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
		if (confirmDisp(GFX_TOP, "Would you like to save all changes\nmade to your save before exiting?")) {
			Commit(false);
		}
	}

	if (R_SUCCEEDED(FSFILE_Close(*m_handle))) {
		FSUSER_ControlArchive(m_archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
	}

	FSUSER_CloseArchive(m_archive);
}