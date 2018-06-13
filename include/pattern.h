#pragma once
#include "save.h"
#include "player.h"

class Pattern {
public:
	Pattern(Save *saveFile, Player *player, u32 id);
	~Pattern();

	void Write(Save *saveFile);

	const u32 Index;
	std::u16string Name;
	std::u16string CreatorName;
	u8 *Palette = new u8[16]; // only the first 15 colors are valid
	u8 *PatternData = nullptr;
	u32 **ImageData = nullptr;
	const u32 Offset;

private:
	u32 ** Decompress(u8 *Data);
	u8 * Compress(u32 **Data);
};