#include "pattern.h"
#include "player.h"

static const u32 PaletteColors[] = {
    0xFFEFFFFF, 0xFF9AADFF, 0xEF559CFF, 0xFF65ADFF, 0xFF0063FF, 0xBD4573FF, 0xCE0052FF, 0x9C0031FF, 0x522031FF, 0x000009FF, 0x00000AFF, 0x00000BFF, 0x00000CFF, 0x00000DFF, 0x00000EFF, 0xFFFFFFFF,
    0xFFBACEFF, 0xFF7573FF, 0xDE3010FF, 0xFF5542FF, 0xFF0000FF, 0xCE6563FF, 0xBD4542FF, 0xBD0000FF, 0x8C2021FF, 0x000019FF, 0x00001AFF, 0x00001BFF, 0x00001CFF, 0x00001DFF, 0x00001EFF, 0xECECECFF,
    0xDECFBDFF, 0xFFCF63FF, 0xDE6521FF, 0xFFAA21FF, 0xFF6500FF, 0xBD8A52FF, 0xDE4500FF, 0xBD4500FF, 0x633010FF, 0x000029FF, 0x00002AFF, 0x00002BFF, 0x00002CFF, 0x00002DFF, 0x00002EFF, 0xDADADAFF,
    0xFFEFDEFF, 0xFFDFCEFF, 0xFFCFADFF, 0xFFBA8CFF, 0xFFAA8CFF, 0xDE8A63FF, 0xBD6542FF, 0x9C5531FF, 0x8C4521FF, 0x000039FF, 0x00003AFF, 0x00003BFF, 0x00003CFF, 0x00003DFF, 0x00003EFF, 0xC8C8C8FF,
    0xFFCFFFFF, 0xEF8AFFFF, 0xCE65DEFF, 0xBD8ACEFF, 0xCE00FFFF, 0x9C659CFF, 0x8C00ADFF, 0x520073FF, 0x310042FF, 0x000049FF, 0x00004AFF, 0x00004BFF, 0x00004CFF, 0x00004DFF, 0x00004EFF, 0xB6B6B6FF,
    0xFFBAFFFF, 0xFF9AFFFF, 0xDE20BDFF, 0xFF55EFFF, 0xFF00CEFF, 0x8C5573FF, 0xBD009CFF, 0x8C0063FF, 0x520042FF, 0x000059FF, 0x00005AFF, 0x00005BFF, 0x00005CFF, 0x00005DFF, 0x00005EFF, 0xA3A3A3FF,
    0xDEBA9CFF, 0xCEAA73FF, 0x734531FF, 0xAD7542FF, 0x9C3000FF, 0x733021FF, 0x522000FF, 0x311000FF, 0x211000FF, 0x000069FF, 0x00006AFF, 0x00006BFF, 0x00006CFF, 0x00006DFF, 0x00006EFF, 0x919191FF,
    0xFFFFCEFF, 0xFFFF73FF, 0xDEDF21FF, 0xFFFF00FF, 0xFFDF00FF, 0xCEAA00FF, 0x9C9A00FF, 0x8C7500FF, 0x525500FF, 0x000079FF, 0x00007AFF, 0x00007BFF, 0x00007CFF, 0x00007DFF, 0x00007EFF, 0x7F7F7FFF,
    0xDEBAFFFF, 0xBD9AEFFF, 0x6330CEFF, 0x9C55FFFF, 0x6300FFFF, 0x52458CFF, 0x42009CFF, 0x210063FF, 0x211031FF, 0x000089FF, 0x00008AFF, 0x00008BFF, 0x00008CFF, 0x00008DFF, 0x00008EFF, 0x6D6D6DFF,
    0xBDBAFFFF, 0x8C9AFFFF, 0x3130ADFF, 0x3155EFFF, 0x0000FFFF, 0x31308CFF, 0x0000ADFF, 0x101063FF, 0x000021FF, 0x000099FF, 0x00009AFF, 0x00009BFF, 0x00009CFF, 0x00009DFF, 0x00009EFF, 0x5B5B5BFF,
    0x9CEFBDFF, 0x63CF73FF, 0x216510FF, 0x42AA31FF, 0x008A31FF, 0x527552FF, 0x215500FF, 0x103021FF, 0x002010FF, 0x0000A9FF, 0x0000AAFF, 0x0000ABFF, 0x0000ACFF, 0x0000ADFF, 0x0000AEFF, 0x484848FF,
    0xDEFFBDFF, 0xCEFF8CFF, 0x8CAA52FF, 0xADDF8CFF, 0x8CFF00FF, 0xADBA9CFF, 0x63BA00FF, 0x529A00FF, 0x316500FF, 0x0000B9FF, 0x0000BAFF, 0x0000BBFF, 0x0000BCFF, 0x0000BDFF, 0x0000BEFF, 0x363636FF,
    0xBDDFFFFF, 0x73CFFFFF, 0x31559CFF, 0x639AFFFF, 0x1075FFFF, 0x4275ADFF, 0x214573FF, 0x002073FF, 0x001042FF, 0x0000C9FF, 0x0000CAFF, 0x0000CBFF, 0x0000CCFF, 0x0000CDFF, 0x0000CEFF, 0x242424FF,
    0xADFFFFFF, 0x52FFFFFF, 0x008ABDFF, 0x52BACEFF, 0x00CFFFFF, 0x429AADFF, 0x00658CFF, 0x004552FF, 0x002031FF, 0x0000D9FF, 0x0000DAFF, 0x0000DBFF, 0x0000DCFF, 0x0000DDFF, 0x0000DEFF, 0x121212FF,
    0xCEFFEFFF, 0xADEFDEFF, 0x31CFADFF, 0x52EFBDFF, 0x00FFCEFF, 0x73AAADFF, 0x00AA9CFF, 0x008A73FF, 0x004531FF, 0x0000E9FF, 0x0000EAFF, 0x0000EBFF, 0x0000ECFF, 0x0000EDFF, 0x0000EEFF, 0x000000FF,
    0xADFFADFF, 0x73FF73FF, 0x63DF42FF, 0x00FF00FF, 0x21DF21FF, 0x52BA52FF, 0x00BA00FF, 0x008A00FF, 0x214521FF, 0x0000F9FF, 0x0000FAFF, 0x0000FBFF, 0x0000FCFF, 0x0000FDFF, 0x0000FEFF, 0x0000FFFF,
};

Pattern::Pattern(Save *saveFile, Player *player, u32 id) :
    Index(id), Offset(player->m_offset + 0x2C + id * 0x870)
{
    Name = saveFile->ReadString(Offset, 20);
    CreatorId = saveFile->ReadU16(Offset + 0x2A);
    CreatorName = saveFile->ReadString(Offset + 0x2C, 8);
    CreatorGender = saveFile->ReadU16(Offset + 0x3E);
    OriginatingTownId = saveFile->ReadU16(Offset + 0x40);
    OriginatingTownName = saveFile->ReadString(Offset + 0x42, 8);
    saveFile->ReadArray(Palette.data(), Offset + 0x58, 16);
    Type = (DesignType)(saveFile->ReadU8(Offset + 0x69) & 9); // TODO: Check the real max disgn type value and limit it to that.
    saveFile->ReadArray(PatternData.data(), Offset + 0x6C, 0x800);

    Decompress();
}

Pattern::~Pattern() {

    for (u32 *data : ImageData)
        linearFree(data);

    for (C2D_Image& image : Images)
        C2D_ImageDelete(image);

}

void Pattern::TakeOwnership(Player *player) {
    CreatorId = player->PlayerId;
    CreatorName = player->Name;
    CreatorGender = player->Gender;

    OriginatingTownId = player->TownId;
    OriginatingTownName = player->TownName;
}

void Pattern::Write(Save *saveFile) {
    Compress();
    saveFile->Write(Offset, Name, 20);
    saveFile->Write(Offset + 0x2A, CreatorId);
    saveFile->Write(Offset + 0x2C, CreatorName, 8);
    saveFile->Write(Offset + 0x3E, CreatorGender);
    saveFile->Write(Offset + 0x40, OriginatingTownId);
    saveFile->Write(Offset + 0x42, OriginatingTownName, 8);
    saveFile->Write(Offset + 0x58, Palette.data(), 16);
    saveFile->Write(Offset + 0x69, (u8)Type);
    saveFile->Write(Offset + 0x6C, PatternData.data(), 0x800);
}

void Pattern::Decompress(void) {

    for (u32 *data : ImageData)
        linearFree(data);
    ImageData.clear();

    for (u32 i = 0; i < 4; ++i)
    {
        u32     idx = i * 0x200;
        u32     outputIdx = 0;
        u32   * output = (u32 *)linearAlloc(sizeof(u32) * 32 * 32);

        for (u32 x = 0; x < 0x200; ++x, ++idx)
        {
            u8  data = PatternData[idx];

            output[outputIdx++] = PaletteColors[Palette[(data & 0x0F)]]; // Left Pixel
            output[outputIdx++] = PaletteColors[Palette[((data >> 4) & 0x0F)]]; // Right Pixel
        }

        Images[i] = ImageDataToC2DImage(output, 32, 32, GPU_RGBA8);
        ImageData.push_back(output);
    }
}

void Pattern::Compress(void) {

    for (int i = 0; i < 4; i++) {
        u32 readOffset = 0;
        u32 idx = i * 200;

        for (int x = 0; x < 0x200; x++) {
            u8 CompressedPixel = 0;

            for (int y = 0; y < 2; y++) {
                u32 Pixel = ImageData[i][readOffset++];

                for (int p = 0; p < 16; p++) {
                    if (PaletteColors[Palette[p]] == Pixel) {
                        CompressedPixel |= (p << (y * 4));
                    }
                }
            }

            PatternData[idx + x] = CompressedPixel;
        }
    }
}