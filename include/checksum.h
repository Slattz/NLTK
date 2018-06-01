#pragma once
#ifndef CHECKSUM
#define CHECKSUM
#include <3ds.h>
#include "save.h"

//class Save;

typedef enum
{
    CHECKSUM_1 = 0,
    CHECKSUM_2 = 1
} ChecksumType;

u32 CalculateCRC32Type1(u8 *buf, u32 size);
u32 CalculateCRC32Type2(u8 *buf, u32 size);
bool VerifyCRC32(u32 crc, u8 *buf, u32 startOffset, u32 size, ChecksumType type = CHECKSUM_1);
u32 UpdateCRC32(Save *save, u32 startOffset, u32 size, ChecksumType type = CHECKSUM_1);
void FixCRC32s(Save *save);

#endif