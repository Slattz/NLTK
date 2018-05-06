#include <turbojpeg.h>
#include "common.h"

typedef struct jpeg_info
{
    unsigned width;
    unsigned height;
    void* image;
} jpeg_info;

jpeg_info decompress_jpeg(const void* buffer, long jpegSize)
{
    int jpegSubsamp, width, height;
    u64 texSize;
    u8* outbuf;
    
    tjhandle jpegDecompressor = tjInitDecompress();
    tjDecompressHeader2(jpegDecompressor, (unsigned char*)buffer, jpegSize, &width, &height, &jpegSubsamp); //For getting width & height of image

    texSize = TJBUFSIZE(width, height);
    outbuf = (u8 *)malloc(texSize);

    tjDecompress2(jpegDecompressor, (unsigned char*)buffer, jpegSize, outbuf, width, 0, height, TJPF_ABGR, TJFLAG_FASTDCT); //Decompress image

    jpeg_info jpeg = {static_cast<unsigned int>(width), static_cast<unsigned int>(height), outbuf};
    tjDestroy(jpegDecompressor);
    return jpeg;
}

bool load_texture_jpeg_memory(size_t id, void* buf, size_t buf_size, u32 maxwidth, u32 maxheight)
{
	if (id >= MAX_TEXTURES)
		return false;

	void* image;
	unsigned width, height;

	jpeg_info jpeg = decompress_jpeg(buf, buf_size);

    width = jpeg.width;
    height = jpeg.height;
    image = jpeg.image;

    if (maxwidth && width > maxwidth)
    {
        return false;
    }

    else if (maxheight && height > maxheight)
    {
        return false;
    }

	pp2d_load_texture_memory(id, image, width, height);
    return true;
}