#pragma once
#include "save.h"
#include "player.h"

#include <array>
#include <vector>

class Player;

class Pattern {
public:
    Pattern(Save *saveFile, Player *player, u32 id);
    ~Pattern();

    void Write(Save *saveFile);

    const u32                   Index;
    std::u16string              Name;
    std::u16string              CreatorName;
    std::array<u8, 0x800>       PatternData;
    std::array<u8, 16>          Palette; // only the first 15 colors are valid
    std::vector<u32 *>          ImageData;
    C2D_Image                   Images[4];

    const u32 Offset;

private:
    void Decompress(void);
    void Compress(void);
};
