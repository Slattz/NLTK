#include "pattern.h"

static const u32 PaletteColors[] = {
    0xFFFFEFFF, 0xFFFF9AAD, 0xFFEF559C, 0xFFFF65AD, 0xFFFF0063, 0xFFBD4573, 0xFFCE0052, 0xFF9C0031, 0xFF522031, 0xFF000009, 0xFF00000A, 0xFF00000B, 0xFF00000C, 0xFF00000D, 0xFF00000E, 0xFFFFFFFF,
    0xFFFFBACE, 0xFFFF7573, 0xFFDE3010, 0xFFFF5542, 0xFFFF0000, 0xFFCE6563, 0xFFBD4542, 0xFFBD0000, 0xFF8C2021, 0xFF000019, 0xFF00001A, 0xFF00001B, 0xFF00001C, 0xFF00001D, 0xFF00001E, 0xFFECECEC,
    0xFFDECFBD, 0xFFFFCF63, 0xFFDE6521, 0xFFFFAA21, 0xFFFF6500, 0xFFBD8A52, 0xFFDE4500, 0xFFBD4500, 0xFF633010, 0xFF000029, 0xFF00002A, 0xFF00002B, 0xFF00002C, 0xFF00002D, 0xFF00002E, 0xFFDADADA,
    0xFFFFEFDE, 0xFFFFDFCE, 0xFFFFCFAD, 0xFFFFBA8C, 0xFFFFAA8C, 0xFFDE8A63, 0xFFBD6542, 0xFF9C5531, 0xFF8C4521, 0xFF000039, 0xFF00003A, 0xFF00003B, 0xFF00003C, 0xFF00003D, 0xFF00003E, 0xFFC8C8C8,
    0xFFFFCFFF, 0xFFEF8AFF, 0xFFCE65DE, 0xFFBD8ACE, 0xFFCE00FF, 0xFF9C659C, 0xFF8C00AD, 0xFF520073, 0xFF310042, 0xFF000049, 0xFF00004A, 0xFF00004B, 0xFF00004C, 0xFF00004D, 0xFF00004E, 0xFFB6B6B6,
    0xFFFFBAFF, 0xFFFF9AFF, 0xFFDE20BD, 0xFFFF55EF, 0xFFFF00CE, 0xFF8C5573, 0xFFBD009C, 0xFF8C0063, 0xFF520042, 0xFF000059, 0xFF00005A, 0xFF00005B, 0xFF00005C, 0xFF00005D, 0xFF00005E, 0xFFA3A3A3,
    0xFFDEBA9C, 0xFFCEAA73, 0xFF734531, 0xFFAD7542, 0xFF9C3000, 0xFF733021, 0xFF522000, 0xFF311000, 0xFF211000, 0xFF000069, 0xFF00006A, 0xFF00006B, 0xFF00006C, 0xFF00006D, 0xFF00006E, 0xFF919191,
    0xFFFFFFCE, 0xFFFFFF73, 0xFFDEDF21, 0xFFFFFF00, 0xFFFFDF00, 0xFFCEAA00, 0xFF9C9A00, 0xFF8C7500, 0xFF525500, 0xFF000079, 0xFF00007A, 0xFF00007B, 0xFF00007C, 0xFF00007D, 0xFF00007E, 0xFF7F7F7F,
    0xFFDEBAFF, 0xFFBD9AEF, 0xFF6330CE, 0xFF9C55FF, 0xFF6300FF, 0xFF52458C, 0xFF42009C, 0xFF210063, 0xFF211031, 0xFF000089, 0xFF00008A, 0xFF00008B, 0xFF00008C, 0xFF00008D, 0xFF00008E, 0xFF6D6D6D,
    0xFFBDBAFF, 0xFF8C9AFF, 0xFF3130AD, 0xFF3155EF, 0xFF0000FF, 0xFF31308C, 0xFF0000AD, 0xFF101063, 0xFF000021, 0xFF000099, 0xFF00009A, 0xFF00009B, 0xFF00009C, 0xFF00009D, 0xFF00009E, 0xFF5B5B5B,
    0xFF9CEFBD, 0xFF63CF73, 0xFF216510, 0xFF42AA31, 0xFF008A31, 0xFF527552, 0xFF215500, 0xFF103021, 0xFF002010, 0xFF0000A9, 0xFF0000AA, 0xFF0000AB, 0xFF0000AC, 0xFF0000AD, 0xFF0000AE, 0xFF484848,
    0xFFDEFFBD, 0xFFCEFF8C, 0xFF8CAA52, 0xFFADDF8C, 0xFF8CFF00, 0xFFADBA9C, 0xFF63BA00, 0xFF529A00, 0xFF316500, 0xFF0000B9, 0xFF0000BA, 0xFF0000BB, 0xFF0000BC, 0xFF0000BD, 0xFF0000BE, 0xFF363636,
    0xFFBDDFFF, 0xFF73CFFF, 0xFF31559C, 0xFF639AFF, 0xFF1075FF, 0xFF4275AD, 0xFF214573, 0xFF002073, 0xFF001042, 0xFF0000C9, 0xFF0000CA, 0xFF0000CB, 0xFF0000CC, 0xFF0000CD, 0xFF0000CE, 0xFF242424,
    0xFFADFFFF, 0xFF52FFFF, 0xFF008ABD, 0xFF52BACE, 0xFF00CFFF, 0xFF429AAD, 0xFF00658C, 0xFF004552, 0xFF002031, 0xFF0000D9, 0xFF0000DA, 0xFF0000DB, 0xFF0000DC, 0xFF0000DD, 0xFF0000DE, 0xFF121212,
    0xFFCEFFEF, 0xFFADEFDE, 0xFF31CFAD, 0xFF52EFBD, 0xFF00FFCE, 0xFF73AAAD, 0xFF00AA9C, 0xFF008A73, 0xFF004531, 0xFF0000E9, 0xFF0000EA, 0xFF0000EB, 0xFF0000EC, 0xFF0000ED, 0xFF0000EE, 0xFF000000,
    0xFFADFFAD, 0xFF73FF73, 0xFF63DF42, 0xFF00FF00, 0xFF21DF21, 0xFF52BA52, 0xFF00BA00, 0xFF008A00, 0xFF214521, 0xFF0000F9, 0xFF0000FA, 0xFF0000FB, 0xFF0000FC, 0xFF0000FD, 0xFF0000FE, 0xFF0000FF
};

Pattern::Pattern(Save *saveFile, Player *player, u32 id) :
    Index(id), Offset(player->m_offset + 0x2C + id * 0x870)
{
    Name = saveFile->ReadString(Offset, 20);
    CreatorName = saveFile->ReadString(Offset + 0x2C, 8);
    saveFile->ReadArray(Palette.data(), Offset + 0x58, 16);
    saveFile->ReadArray(PatternData.data(), Offset + 0x6C, 0x800);

    Decompress();
}

Pattern::~Pattern() {
    for (u32 *data : ImageData)
        linearFree(data);

    for (C2D_Image& image : Images)
    {
        C3D_TexDelete(image.tex);
        delete image.tex;
        delete image.subtex;
    }

}

void Pattern::Write(Save *saveFile) {
    //Compress(ImageData);
    saveFile->Write(Offset, Name, 20);
    saveFile->Write(Offset + 0x2C, CreatorName, 8);
    saveFile->Write(Offset + 0x58, Palette.data(), 16);
    saveFile->Write(Offset + 0x6C, PatternData.data(), 0x800);
}

void    Pattern::Decompress(void) {
    for (u32 *data : ImageData)
        linearFree(data);
    ImageData.clear();

    for (u32 i = 0; i < 4; ++i)
    {
        u32     idx = i * 0x200;
        u32     outputIdx = 0;
        u32   * output = (u32 *)linearAlloc(sizeof(u32) * 0x400);
        //MsgDisp(top, Format("Creating pattern: %d at offset: %08X", i, (u32)output));

        for (u32 x = 0; x < 0x200; ++x, ++idx)
        {
            u8  data = PatternData[idx];

            output[outputIdx++] = __builtin_bswap32(PaletteColors[Palette[((data >> 4) & 0x0F)]]); // Left Pixel
            output[outputIdx++] = __builtin_bswap32(PaletteColors[Palette[(data & 0x0F)]]); // Right Pixel
            //if (outputIdx > 0x400)
            //    MsgDisp(top, Format("outputIdx >= 0x400! idx: %04X", idx));
        }

        //MsgDisp(top, Format("Creating Pattern Image for pattern: %d", i));

        Images[i] = ImageDataToC2DImage(output, 32, 32, GPU_RGBA8);
        ImageData.push_back(output);
    }
}

u8 * Pattern::Compress(u32 ** Data) {
    /*if (Data != nullptr) {
        if (PatternData != nullptr) {
            delete[] PatternData;
        }

        u8 *Output = new u8[0x800];
        for (int i = 0; i < 4; i++) {
            u32 readOffset = 0;
            u32 idx = i * 200;
            for (int x = 0; x < 0x200; x++) {
                u8 CompressedPixel = 0;
                for (int y = 0; y < 2; y++) {
                    u32 Pixel = Data[i][readOffset++];
                    for (int p = 0; p < 16; p++) {
                        if (PaletteColors[Palette[p]] == Pixel) {
                            CompressedPixel |= (p << (y * 4));
                        }
                    }
                }

                Output[idx + x] = CompressedPixel;
            }
        }

        PatternData = Output;
        return Output;
    }
    return nullptr;*/
}