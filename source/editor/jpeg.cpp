#include <3ds.h>
#include <string>
#include <citro2d.h>
#include <turbojpeg.h>
#include "gfx.h"
#include "common.h"
#include "utils.h"
#include "jpeg.h"

#define TPC_SIZE        0x1400
#define TPC_MAX_WIDTH   64
#define TPC_MAX_HEIGHT  104

static inline u32 Pow2(u32 x)
{
    if (x <= 2)
        return x;

    return 1u << (32 - __builtin_clz(x - 1));
}

JPEGInfo DecompressJPEG(const void * jpegSrc, const u32 jpegSize)
{
    JPEGInfo    jpeg = {0, 0, nullptr};
    int         jpegSubsamp;
    tjhandle    jpegDecompressor = tjInitDecompress();

    // Get the dimensions of the image
    if (tjDecompressHeader2(jpegDecompressor, (u8 *)jpegSrc, jpegSize, (int *)&jpeg.width, (int *)&jpeg.height, &jpegSubsamp))
        MsgDisp(top, std::string("tjDecompressHeader2: error\n") + tjGetErrorStr());

    // Allocate the temporary buffer
    u32 powSize = Pow2(jpeg.width) * Pow2(jpeg.height) * 4;
    jpeg.image = linearAlloc(powSize);

    if (!jpeg.image)
        MsgDisp(top, "jpeg.image is null");

    // Clear temporary buffer
    C3D_SyncMemoryFill((u32 *)jpeg.image, 0, (u32 *)((u8 *)jpeg.image + powSize), BIT(0) | GX_FILL_32BIT_DEPTH, nullptr, 0, nullptr, 0);
    GSPGPU_FlushDataCache(jpeg.image, powSize);

    // Decompress the image
    if (tjDecompress2(jpegDecompressor, (u8 *)jpegSrc, jpegSize, (u8 *)jpeg.image, jpeg.width, 0, jpeg.height, TJPF_ABGR, TJFLAG_FASTDCT))
        MsgDisp(top, std::string("tjDecompress2: error\n") + tjGetErrorStr());
    GSPGPU_FlushDataCache(jpeg.image, powSize);

    tjDestroy(jpegDecompressor);

    return jpeg;
}

C2D_Image LoadPlayerPicture(const void *tpcData)
{
    JPEGInfo jpeg = DecompressJPEG(tpcData, TPC_SIZE);

    if (jpeg.width > TPC_MAX_WIDTH || jpeg.height > TPC_MAX_HEIGHT || jpeg.image == nullptr) {
        if (jpeg.image != nullptr)
            linearFree(jpeg.image);
        return C2D_SpriteSheetGetImage(Players_ss, NO_TPC_PIC);
    }

    C2D_Image image = ImageDataToC2DImage((u32*)jpeg.image, jpeg.width, jpeg.height, GPU_RGBA8);
    linearFree(jpeg.image);
    return image;
}
