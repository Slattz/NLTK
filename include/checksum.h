#pragma once
#ifndef CHECKSUM
#define CHECKSUM
#include <3ds.h>
#include "save.h"

//class Save;

u32 CalculateCRC32(u8 *buf, u32 size);
bool VerifyCRC32(u32 crc, u8 *buf, u32 startOffset, u32 size);
u32 UpdateCRC32(Save *save, u32 startOffset, u32 size);
void FixCRC32s(Save *save);

#endif