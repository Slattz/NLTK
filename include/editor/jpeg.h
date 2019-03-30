#pragma once

#ifndef JPEG_H
#define JPEG_H

struct JPEGInfo
{
    u32 width;
    u32 height;
    void *image;
};

JPEGInfo DecompressJPEG(const void * jpegSrc, const u32 jpegSize);
C2D_Image LoadPlayerPicture(const void *tpcData);


#endif