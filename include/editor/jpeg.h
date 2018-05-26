#pragma once

#ifndef JPEG_H
#define JPEG_H

struct jpeg_info decompress_jpeg(const void* buffer, long jpegSize);
C2D_Image LoadPlayerPicture(const void *tpcData);
void C2D_Image_Destroy(C2D_Image image);


#endif