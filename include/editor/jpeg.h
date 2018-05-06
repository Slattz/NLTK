#pragma once

#ifndef JPEG_H
#define JPEG_H

struct jpeg_info decompress_jpeg(const void* buffer, long jpegSize);
bool load_texture_jpeg_memory(size_t id, void* buf, size_t buf_size, u32 maxwidth, u32 maxheight);


#endif