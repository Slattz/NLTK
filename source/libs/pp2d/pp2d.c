/* MIT License
 *
 * Copyright (c) 2017 Bernardo Giordano
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * https://discord.gg/bGKEyfY
 */
 
/**
 * Plug & Play 2D
 * @file pp2d.c
 * @author Bernardo Giordano
 * @date 17 January 2018
 * @brief pp2d implementation
 */

#include "pp2d.h"
#include "loadbmp.h"

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection;
static C3D_Mtx projectionTopLeft;
static C3D_Mtx projectionTopRight;
static C3D_Mtx projectionBot;
static textVertex_s* textVtxArray;
static int textVtxArrayPos;
static C3D_RenderTarget* topLeft;
static C3D_RenderTarget* topRight;
static C3D_RenderTarget* bot;
static const BCFNT_s  *g_font;
static BCFNT_s  *g_sysfont;

static struct {
    GPU_TEXTURE_FILTER_PARAM magFilter;
    GPU_TEXTURE_FILTER_PARAM minFilter;
} textureFilters;

static struct {
    size_t id;
    int x;
    int y;
    int xbegin;
    int ybegin;
    int width;
    int height;
    u32 color;
    flipType fliptype;
    float scaleX;
    float scaleY;
    float angle;
    float depth;
    bool initialized;
} textureData;

static struct {
    C3D_Tex tex;
    u32 width;
    u32 height;
    bool allocated;
} textures[MAX_TEXTURES];

static void pp2d_add_text_vertex(float vx, float vy, float vz, float tx, float ty);
static bool pp2d_add_quad(int x, int y, int height, int width, float left, float right, float top, float bottom, float depth);
static u32 pp2d_get_next_pow2(u32 n);
static void pp2d_get_text_size_internal(float* width, float* height, float scaleX, float scaleY, int wrapX, const char* text);
static void pp2d_set_text_color(u32 color);
static Result bcfnt_create_glyphsheet(BCFNT_s *font);

static void pp2d_add_text_vertex(float vx, float vy, float vz, float tx, float ty)
{
    textVertex_s* vtx = &textVtxArray[textVtxArrayPos++];
    vtx->position[0] = vx;
    vtx->position[1] = vy;
    vtx->position[2] = vz;
    vtx->texcoord[0] = tx;
    vtx->texcoord[1] = ty;
}

void pp2d_begin_draw(gfxScreen_t target, gfx3dSide_t side)
{
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    textVtxArrayPos = 0;
    pp2d_draw_on(target, side);
}

void pp2d_draw_on(gfxScreen_t target, gfx3dSide_t side)
{
    if(target == GFX_TOP) {
        C3D_FrameDrawOn(side == GFX_LEFT ? topLeft : topRight);
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, side == GFX_LEFT ? &projectionTopLeft : &projectionTopRight);
    } else {
        C3D_FrameDrawOn(bot);
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projectionBot);
    }
}

void pp2d_draw_rectangle(int x, int y, int width, int height, u32 color)
{
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_CONSTANT, GPU_CONSTANT, 0);
    C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
    C3D_TexEnvFunc(env, C3D_RGB, GPU_INTERPOLATE);
    C3D_TexEnvColor(env, color);
    
    if (pp2d_add_quad(x, y, height, width, 0, 0, 0, 0, DEFAULT_DEPTH))
    {
        C3D_DrawArrays(GPU_TRIANGLE_STRIP, textVtxArrayPos - 4, 4);
    }
}

void pp2d_draw_text(float x, float y, float scaleX, float scaleY, u32 color, const char* text)
{
    pp2d_draw_text_wrap(x, y, scaleX, scaleY, color, -1, text);
}

void pp2d_draw_text_center(gfxScreen_t target, float y, float scaleX, float scaleY, u32 color, const char* text)
{
    float width = pp2d_get_text_width(text, scaleX, scaleY);
    float x = ((target == GFX_TOP ? TOP_WIDTH : BOTTOM_WIDTH) - width) / 2;
    pp2d_draw_text(x, y, scaleX, scaleY, color, text);
}

void pp2d_draw_textf(float x, float y, float scaleX, float scaleY, u32 color, const char* text, ...) 
{
    char buffer[256];
    va_list args;
    va_start(args, text);
    vsnprintf(buffer, 256, text, args);
    pp2d_draw_text(x, y, scaleX, scaleY, color, buffer);
    va_end(args);
}

void pp2d_draw_text_wrap(float x, float y, float scaleX, float scaleY, u32 color, float wrapX, const char* text)
{
    if (text == NULL)
        return;
    
    pp2d_set_text_color(color);

    ssize_t  units;
    uint32_t code;
    const uint8_t* p = (const uint8_t*)text;
    float firstX = x;
    int lastSheet = -1;
    C3D_Tex *glyphSheets = g_font->glyphSheets;
    
    scaleX *= g_font->textScale;
    scaleY *= g_font->textScale;
    
    do
    {
        if (!*p) break;
        units = decode_utf8(&code, p);
        if (units == -1)
            break;
        p += units;
        if (code == '\n' || (wrapX != -1 && x + scaleX * pp2d_fontGetCharWidthInfo(pp2d_fontGlyphIndexFromCodePoint(code))->charWidth >= firstX + wrapX))
        {
            x = firstX;
            y += scaleY*pp2d_fontGetInfo()->lineFeed;
            p -= code == '\n' ? 0 : 1;
        }
        else if (code > 0)
        {
            int glyphIdx = pp2d_fontGlyphIndexFromCodePoint(code);
            fontGlyphPos_s data;
            pp2d_fontCalcGlyphPos(&data, glyphIdx, GLYPH_POS_CALC_VTXCOORD, scaleX, scaleY);

            if (data.sheetIndex != lastSheet)
            {
                lastSheet = data.sheetIndex;
                C3D_TexBind(0, &glyphSheets[lastSheet]);
            }

            if ((textVtxArrayPos+4) >= TEXT_VTX_ARRAY_COUNT)
                break;

            pp2d_add_text_vertex(x+data.vtxcoord.left,  y+data.vtxcoord.bottom, DEFAULT_DEPTH, data.texcoord.left,  data.texcoord.bottom);
            pp2d_add_text_vertex(x+data.vtxcoord.right, y+data.vtxcoord.bottom, DEFAULT_DEPTH, data.texcoord.right, data.texcoord.bottom);
            pp2d_add_text_vertex(x+data.vtxcoord.left,  y+data.vtxcoord.top,    DEFAULT_DEPTH, data.texcoord.left,  data.texcoord.top);
            pp2d_add_text_vertex(x+data.vtxcoord.right, y+data.vtxcoord.top,    DEFAULT_DEPTH, data.texcoord.right, data.texcoord.top);

            C3D_DrawArrays(GPU_TRIANGLE_STRIP, textVtxArrayPos - 4, 4);

            x += data.xAdvance;
        }
    } while (code > 0);
}

void pp2d_draw_texture(size_t id, int x, int y)
{
    pp2d_texture_select(id, x, y);
    pp2d_texture_draw();
}

void pp2d_draw_texture_blend(size_t id, int x, int y, u32 color)
{
    pp2d_texture_select(id, x, y);
    pp2d_texture_blend(color);
    pp2d_texture_draw();
}

void pp2d_draw_texture_flip(size_t id, int x, int y, flipType fliptype)
{
    pp2d_texture_select(id, x, y);
    pp2d_texture_flip(fliptype);
    pp2d_texture_draw();
}

void pp2d_draw_texture_rotate(size_t id, int x, int y, float angle)
{
    pp2d_texture_select(id, x, y);
    pp2d_texture_rotate(angle);
    pp2d_texture_draw();        
}

void pp2d_draw_texture_scale(size_t id, int x, int y, float scaleX, float scaleY)
{
    pp2d_texture_select(id, x, y);
    pp2d_texture_scale(scaleX, scaleY);
    pp2d_texture_draw();
}

void pp2d_draw_texture_part(size_t id, int x, int y, int xbegin, int ybegin, int width, int height)
{
    pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
    pp2d_texture_draw();
}

void pp2d_draw_wtext(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t* text) 
{
    pp2d_draw_wtext_wrap(x, y, scaleX, scaleY, color, -1, text);
}

void pp2d_draw_wtext_center(gfxScreen_t target, float y, float scaleX, float scaleY, u32 color, const wchar_t* text)
{
    float width = pp2d_get_wtext_width(text, scaleX, scaleY);
    float x = ((target == GFX_TOP ? TOP_WIDTH : BOTTOM_WIDTH) - width) / 2;
    pp2d_draw_wtext(x, y, scaleX, scaleY, color, text);
}

void pp2d_draw_wtext_wrap(float x, float y, float scaleX, float scaleY, u32 color, float wrapX, const wchar_t* text) 
{
    if (text == NULL)
        return;
    
    u32 size = wcslen(text) * sizeof(wchar_t);
    char buf[size];
    memset(buf, 0, size);
    utf32_to_utf8((uint8_t*)buf, (uint32_t*)text, size);
    buf[size - 1] = '\0';   
    
    pp2d_draw_text_wrap(x, y, scaleX, scaleY, color, wrapX, buf);
}

void pp2d_draw_wtextf(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t* text, ...) 
{
    wchar_t buffer[256];
    va_list args;
    va_start(args, text);
    vswprintf(buffer, 256, text, args);
    pp2d_draw_wtext(x, y, scaleX, scaleY, color, buffer);
    va_end(args);
}

void pp2d_end_draw(void)
{
    C3D_FrameEnd(0);
}

void pp2d_exit(void)
{
    for (size_t id = 0; id < MAX_TEXTURES; id++)
    {
        pp2d_free_texture(id);
    }
    
    linearFree(textVtxArray);
    free(g_sysfont->glyphSheets);
    free(g_sysfont);
    
    shaderProgramFree(&program);
    DVLB_Free(vshader_dvlb);
    
    C3D_Fini();
    gfxExit();
}

static bool pp2d_add_quad(int x, int y, int height, int width, float left, float right, float top, float bottom, float depth)
{
    if ((textVtxArrayPos+4) >= TEXT_VTX_ARRAY_COUNT)
        return false;
    
    pp2d_add_text_vertex(        x, y + height, depth,  left, bottom);
    pp2d_add_text_vertex(x + width, y + height, depth, right, bottom);
    pp2d_add_text_vertex(        x,          y, depth,  left,    top);
    pp2d_add_text_vertex(x + width,          y, depth, right,    top);
    
    return true;
}

void pp2d_free_texture(size_t id)
{
    if (id >= MAX_TEXTURES)
        return;
    
    if (!textures[id].allocated)
        return;
    
    C3D_TexDelete(&textures[id].tex);
    textures[id].width = 0;
    textures[id].height = 0;
    textures[id].allocated = false;
}

Result pp2d_init(void)
{
    Result res = 0;
    
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    
    topLeft = C3D_RenderTargetCreate(SCREEN_HEIGHT, TOP_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetClear(topLeft, C3D_CLEAR_ALL, BACKGROUND_COLOR, 0);
    C3D_RenderTargetSetOutput(topLeft, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
    
    topRight = C3D_RenderTargetCreate(SCREEN_HEIGHT, TOP_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetClear(topRight, C3D_CLEAR_ALL, BACKGROUND_COLOR, 0);
    C3D_RenderTargetSetOutput(topRight, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);
    
    bot = C3D_RenderTargetCreate(SCREEN_HEIGHT, BOTTOM_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetClear(bot, C3D_CLEAR_ALL, BACKGROUND_COLOR, 0);
    C3D_RenderTargetSetOutput(bot, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
    
    res = fontEnsureMapped();
    if (R_FAILED(res))
        return res;
    
    pp2d_set_texture_filter(GPU_NEAREST, GPU_NEAREST);

#ifdef BUILDTOOLS
    vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_len);
#else
    vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
#endif

    shaderProgramInit(&program);
    shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
    C3D_BindProgram(&program);

    uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
    
    C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3);
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2);

    Mtx_OrthoTilt(&projectionTopLeft, 0, TOP_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, true);
    Mtx_OrthoTilt(&projectionTopRight, 0, TOP_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, true);
    Mtx_OrthoTilt(&projectionBot, 0, BOTTOM_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, true);
    
    C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

    g_sysfont = malloc(sizeof(BCFNT_s));
    extern CFNT_s *g_sharedFont;

    g_sysfont->cfnt = g_sharedFont;
    g_sysfont->charPerSheet = g_sharedFont->finf.tglp->nRows * g_sharedFont->finf.tglp->nLines;
    g_font = g_sysfont;

    bcfnt_create_glyphsheet(g_sysfont);
    
    charWidthInfo_s* cwi = pp2d_fontGetCharWidthInfo(pp2d_fontGlyphIndexFromCodePoint(0x3042));
    g_sysfont->textScale = 20.0f / (cwi->glyphWidth); // 20 is glyphWidth in J machines
    
    textVtxArray = (textVertex_s*)linearAlloc(sizeof(textVertex_s)*TEXT_VTX_ARRAY_COUNT);
    C3D_BufInfo* bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, textVtxArray, sizeof(textVertex_s), 2, 0x10);
    
    return 0;
}

static u32 pp2d_get_next_pow2(u32 v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v >= 64 ? v : 64;
}

float pp2d_get_text_height(const char* text, float scaleX, float scaleY)
{
    float height;
    pp2d_get_text_size_internal(NULL, &height, scaleX, scaleY, -1, text);
    return height;
}

float pp2d_get_text_height_wrap(const char* text, float scaleX, float scaleY, int wrapX)
{
    float height;
    pp2d_get_text_size_internal(NULL, &height, scaleX, scaleY, wrapX, text);
    return height;
}

void pp2d_get_text_size(float* width, float* height, float scaleX, float scaleY, const char* text)
{
    pp2d_get_text_size_internal(width, height, scaleX, scaleY, -1, text);
}

static void pp2d_get_text_size_internal(float* width, float* height, float scaleX, float scaleY, int wrapX, const char* text)
{
    float maxW = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
    
    ssize_t  units;
    uint32_t code;
    float x = 0;
    float firstX = x;
    const uint8_t* p = (const uint8_t*)text;
    
    scaleX *= g_font->textScale;
    scaleY *= g_font->textScale;
    
    do
    {
        if (!*p) break;
        units = decode_utf8(&code, p);
        if (units == -1)
            break;
        p += units;
        if (code == '\n' || (wrapX != -1 && x + scaleX * pp2d_fontGetCharWidthInfo(pp2d_fontGlyphIndexFromCodePoint(code))->charWidth >= firstX + wrapX))
        {
            x = firstX;
            h += scaleY*pp2d_fontGetInfo()->lineFeed;
            p -= code == '\n' ? 0 : 1;
            if (w > maxW)
                maxW = w;
            w = 0.f;
        }
        else if (code > 0)
        {
            float len = (scaleX * pp2d_fontGetCharWidthInfo(pp2d_fontGlyphIndexFromCodePoint(code))->charWidth);
            w += len;
            x += len;
        }
    } while (code > 0);
    
    if (width)
    {
        *width = w > maxW ? w : maxW;
    }
    
    if (height)
    {
        h += scaleY*pp2d_fontGetInfo()->lineFeed;
        *height = h;
    }
}

float pp2d_get_text_width(const char* text, float scaleX, float scaleY)
{
    float width;
    pp2d_get_text_size_internal(&width, NULL, scaleX, scaleY, -1, text);
    return width;
}

float pp2d_get_wtext_height(const wchar_t* text, float scaleX, float scaleY)
{
    u32 size = wcslen(text) * sizeof(wchar_t);
    char buf[size];
    memset(buf, 0, size);
    utf32_to_utf8((uint8_t*)buf, (uint32_t*)text, size);
    buf[size - 1] = '\0';
    
    float height;
    pp2d_get_text_size_internal(NULL, &height, scaleX, scaleY, -1, buf);
    return height;
}

float pp2d_get_wtext_width(const wchar_t* text, float scaleX, float scaleY)
{
    u32 size = wcslen(text) * sizeof(wchar_t);
    char buf[size];
    memset(buf, 0, size);
    utf32_to_utf8((uint8_t*)buf, (uint32_t*)text, size);
    buf[size - 1] = '\0';
    
    float width;
    pp2d_get_text_size_internal(&width, NULL, scaleX, scaleY, -1, buf);
    return width;
}

void pp2d_load_texture_bmp(size_t id, const char* path)
{
    if (id >= MAX_TEXTURES)
        return;
    
    u8* image = NULL;
    unsigned int width = 0, height = 0;
    loadbmp_decode_file(path, &image, &width, &height);
    
    pp2d_load_texture_memory(id, image, width, height);
    free(image);
}

void pp2d_load_texture_memory(size_t id, void* buf, u32 width, u32 height)
{
    u32 w_pow2 = pp2d_get_next_pow2(width);
    u32 h_pow2 = pp2d_get_next_pow2(height);
    
    C3D_TexInit(&textures[id].tex, (u16)w_pow2, (u16)h_pow2, GPU_RGBA8);
    C3D_TexSetFilter(&textures[id].tex, textureFilters.magFilter, textureFilters.minFilter);
    
    textures[id].allocated = true;
    textures[id].width = width;
    textures[id].height = height;

    memset(textures[id].tex.data, 0, textures[id].tex.size);
    for (u32 i = 0; i < width; i++) 
    {
        for (u32 j = 0; j < height; j++) 
        {
            u32 dst = ((((j >> 3) * (w_pow2 >> 3) + (i >> 3)) << 6) + ((i & 1) | ((j & 1) << 1) | ((i & 2) << 1) | ((j & 2) << 2) | ((i & 4) << 2) | ((j & 4) << 3))) * 4;
            u32 src = (j * width + i) * 4;

            memcpy(textures[id].tex.data + dst, buf + src, 4);
        }
    }

    C3D_TexFlush(&textures[id].tex);    
}

void pp2d_load_texture_png(size_t id, const char* path)
{
    if (id >= MAX_TEXTURES)
        return;
    
    u8* image;
    unsigned width, height;

    lodepng_decode32_file(&image, &width, &height, path);
    for (u32 i = 0; i < width; i++) 
    {
        for (u32 j = 0; j < height; j++) 
        {
            u32 p = (i + j*width) * 4;

            u8 r = *(u8*)(image + p);
            u8 g = *(u8*)(image + p + 1);
            u8 b = *(u8*)(image + p + 2);
            u8 a = *(u8*)(image + p + 3);

            *(image + p) = a;
            *(image + p + 1) = b;
            *(image + p + 2) = g;
            *(image + p + 3) = r;
        }
    }
    
    pp2d_load_texture_memory(id, image, width, height);
    free(image);
}

void pp2d_load_texture_png_memory(size_t id, void* buf, size_t buf_size)
{
    if (id >= MAX_TEXTURES)
        return;

    u8* image;
    unsigned width;
    unsigned height;

    lodepng_decode32(&image, &width, &height, buf, buf_size);

    for (u32 i = 0; i < width; i++)
    {
        for (u32 j = 0; j < height; j++)
        {
            u32 p = (i + j*width) * 4;

            u8 r = *(u8*)(image + p);
            u8 g = *(u8*)(image + p + 1);
            u8 b = *(u8*)(image + p + 2);
            u8 a = *(u8*)(image + p + 3);

            *(image + p) = a;
            *(image + p + 1) = b;
            *(image + p + 2) = g;
            *(image + p + 3) = r;
        }
    }

    pp2d_load_texture_memory(id, image, width, height);
    free(image);
}

void pp2d_set_3D(int enable)
{
    gfxSet3D(enable);
}

void pp2d_set_screen_color(gfxScreen_t target, u32 color)
{
    if (target == GFX_TOP)
    {
        C3D_RenderTargetSetClear(topLeft, C3D_CLEAR_ALL, color, 0);
        C3D_RenderTargetSetClear(topRight, C3D_CLEAR_ALL, color, 0);
    }
    else
    {
        C3D_RenderTargetSetClear(bot, C3D_CLEAR_ALL, color, 0);
    }
}

void pp2d_set_texture_filter(GPU_TEXTURE_FILTER_PARAM magFilter, GPU_TEXTURE_FILTER_PARAM minFilter)
{
    textureFilters.magFilter = magFilter;
    textureFilters.minFilter = minFilter;
}

static void pp2d_set_text_color(u32 color)
{
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_RGB, GPU_CONSTANT, 0, 0);
    C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0, GPU_CONSTANT, 0);
    C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
    C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
    C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
    C3D_TexEnvColor(env, color);
}

void pp2d_texture_select(size_t id, int x, int y)
{
    if (id >= MAX_TEXTURES)
    {
        textureData.initialized = false;
        return;
    }
    
    textureData.id = id;
    textureData.x = x;
    textureData.y = y;
    textureData.xbegin = 0;
    textureData.ybegin = 0;
    textureData.width = textures[id].width;
    textureData.height = textures[id].height;
    textureData.color = PP2D_NEUTRAL;
    textureData.fliptype = NONE;
    textureData.scaleX = 1;
    textureData.scaleY = 1;
    textureData.angle = 0;
    textureData.depth = DEFAULT_DEPTH;
    textureData.initialized = true;
}

void pp2d_texture_select_part(size_t id, int x, int y, int xbegin, int ybegin, int width, int height)
{
    if (id >= MAX_TEXTURES)
    {
        textureData.initialized = false;
        return;
    }
    
    textureData.id = id;
    textureData.x = x;
    textureData.y = y;
    textureData.xbegin = xbegin;
    textureData.ybegin = ybegin;
    textureData.width = width;
    textureData.height = height;
    textureData.color = PP2D_NEUTRAL;
    textureData.fliptype = NONE;
    textureData.scaleX = 1;
    textureData.scaleY = 1;
    textureData.angle = 0;
    textureData.depth = DEFAULT_DEPTH;
    textureData.initialized = true;
}

void pp2d_texture_blend(u32 color)
{
    textureData.color = color;
}

void pp2d_texture_scale(float scaleX, float scaleY)
{
    textureData.scaleX = scaleX;
    textureData.scaleY = scaleY;
}

void pp2d_texture_flip(flipType fliptype)
{
    textureData.fliptype = fliptype;
}

void pp2d_texture_rotate(float angle)
{
    textureData.angle = angle;
}

void pp2d_texture_depth(float depth)
{
    textureData.depth = depth;
}

void pp2d_texture_draw(void)
{
    if (!textureData.initialized)
        return;

    if ((textVtxArrayPos+4) >= TEXT_VTX_ARRAY_COUNT)
        return;
    
    size_t id = textureData.id;
    
    float left = (float)textureData.xbegin / (float)textures[id].tex.width;
    float right = (float)(textureData.xbegin + textureData.width) / (float)textures[id].tex.width;
    float top = (float)(textures[id].tex.height - textureData.ybegin) / (float)textures[id].tex.height;
    float bottom = (float)(textures[id].tex.height - textureData.ybegin - textureData.height) / (float)textures[id].tex.height;
    
    // scaling
    textureData.height *= textureData.scaleY;
    textureData.width *= textureData.scaleX;
    
    float vert[4][2] = {
        {                    textureData.x, textureData.height + textureData.y},
        {textureData.width + textureData.x, textureData.height + textureData.y},
        {                    textureData.x,                      textureData.y},
        {textureData.width + textureData.x,                      textureData.y},
    };

    // flipping
    if (textureData.fliptype == BOTH || textureData.fliptype == HORIZONTAL)
    {
        float tmp = left;
        left = right;
        right = tmp;
    }
    
    if (textureData.fliptype == BOTH || textureData.fliptype == VERTICAL)
    {
        float tmp = top;
        top = bottom;
        bottom = tmp;
    }
    
    // rotating
    textureData.angle = fmod(textureData.angle, 360);
    if (textureData.angle != 0)
    {
        const float rad = textureData.angle/(180/M_PI);
        const float c = cosf(rad);
        const float s = sinf(rad);
        
        const float xcenter = textureData.x + textureData.width/2.0f;
        const float ycenter = textureData.y + textureData.height/2.0f;
        
        for (int i = 0; i < 4; i++)
        {
            float oldx = vert[i][0];
            float oldy = vert[i][1];
            
            float newx = c * (oldx - xcenter) - s * (oldy - ycenter) + xcenter;
            float newy = s * (oldx - xcenter) + c * (oldy - ycenter) + ycenter;
            
            vert[i][0] = newx;
            vert[i][1] = newy;
        }
    }

    // blending
    C3D_TexBind(0, &textures[id].tex);
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_CONSTANT, 0);
    C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
    C3D_TexEnvColor(env, textureData.color);

    // rendering
    pp2d_add_text_vertex(vert[0][0], vert[0][1], textureData.depth, left, bottom);
    pp2d_add_text_vertex(vert[1][0], vert[1][1], textureData.depth, right, bottom);
    pp2d_add_text_vertex(vert[2][0], vert[2][1], textureData.depth, left, top);
    pp2d_add_text_vertex(vert[3][0], vert[3][1], textureData.depth, right, top);

    C3D_DrawArrays(GPU_TRIANGLE_STRIP, textVtxArrayPos - 4, 4);
}


const BCFNT_s*  pp2d_get_sysfont(void)
{
    return (g_sysfont);
}

Result  pp2d_use_font(const BCFNT_s *font)
{
    if (font == NULL || font->cfnt == NULL || font->cfnt->signature != 0x554E4643) ///< CFNU
        return MAKERESULT(RL_USAGE, RS_INVALIDARG, RM_APPLICATION, RD_INVALID_POINTER);

    g_font = font;
    return 0;
}

FINF_s* pp2d_fontGetInfo(void)
{
    return &g_font->cfnt->finf;
}

TGLP_s* pp2d_fontGetGlyphInfo(void)
{
    return g_font->cfnt->finf.tglp;
}

void* pp2d_fontGetGlyphSheetTex(int sheetIndex)
{
    return &g_font->cfnt->finf.tglp->sheetData[sheetIndex*g_font->cfnt->finf.tglp->sheetSize];
}

int pp2d_fontGlyphIndexFromCodePoint(u32 codePoint)
{
    int ret = g_font->cfnt->finf.alterCharIndex;

    if (codePoint < 0x10000)
    {
        CMAP_s* cmap;

        for (cmap = g_font->cfnt->finf.cmap; cmap; cmap = cmap->next)
        {
            if (codePoint < cmap->codeBegin || codePoint > cmap->codeEnd)
                continue;

            if (cmap->mappingMethod == CMAP_TYPE_DIRECT)
            {
                ret = cmap->indexOffset + (codePoint - cmap->codeBegin);
                break;
            }

            if (cmap->mappingMethod == CMAP_TYPE_TABLE)
            {
                ret = cmap->indexTable[codePoint - cmap->codeBegin];
                break;
            }

            int j;
            for (j = 0; j < cmap->nScanEntries; j ++)
                if (cmap->scanEntries[j].code == codePoint)
                    break;
            if (j < cmap->nScanEntries)
            {
                ret = cmap->scanEntries[j].glyphIndex;
                break;
            }
        }
    }
    return ret;
}

charWidthInfo_s* pp2d_fontGetCharWidthInfo(int glyphIndex)
{
    charWidthInfo_s *info = NULL;
    CWDH_s          *cwdh;

    for (cwdh = g_font->cfnt->finf.cwdh; cwdh && !info; cwdh = cwdh->next)
    {
        if (glyphIndex < cwdh->startIndex || glyphIndex > cwdh->endIndex)
            continue;
        info = &cwdh->widths[glyphIndex - cwdh->startIndex];
    }

    if (!info)
        info = &g_font->cfnt->finf.defaultWidth;

    return info;
}

void    pp2d_fontCalcGlyphPos(fontGlyphPos_s* out, int glyphIndex, u32 flags, float scaleX, float scaleY)
{
    FINF_s          *finf = &g_font->cfnt->finf;
    TGLP_s          *tglp = finf->tglp;
    charWidthInfo_s *cwi = pp2d_fontGetCharWidthInfo(glyphIndex);

    int     sheetId = glyphIndex / g_font->charPerSheet;
    int     glInSheet = glyphIndex % g_font->charPerSheet;

    out->sheetIndex = sheetId;
    out->xOffset = scaleX*cwi->left;
    out->xAdvance = scaleX*cwi->charWidth;
    out->width = scaleX*cwi->glyphWidth;

    int lineId = glInSheet / tglp->nRows;
    int rowId = glInSheet % tglp->nRows;

    float tx = (float)(rowId*(tglp->cellWidth+1)+1) / tglp->sheetWidth;
    float ty = 1.0f - (float)(lineId*(tglp->cellHeight+1)+1) / tglp->sheetHeight;
    float tw = (float)cwi->glyphWidth / tglp->sheetWidth;
    float th = (float)tglp->cellHeight / tglp->sheetHeight;
    out->texcoord.left = tx;
    out->texcoord.top = ty;
    out->texcoord.right = tx+tw;
    out->texcoord.bottom = ty-th;

    if (flags & GLYPH_POS_CALC_VTXCOORD)
    {
        float vx = out->xOffset;
        float vy = (flags & GLYPH_POS_AT_BASELINE) ? (scaleY*tglp->baselinePos) : 0;
        float vw = out->width;
        float vh = scaleY*tglp->cellHeight;
        if (flags & GLYPH_POS_Y_POINTS_UP)
        {
            vy = -(vh-vy);
            out->vtxcoord.left = vx;
            out->vtxcoord.top = vy+vh;
            out->vtxcoord.right = vx+vw;
            out->vtxcoord.bottom = vy;
        } else
        {
            vy = -vy;
            out->vtxcoord.left = vx;
            out->vtxcoord.top = vy;
            out->vtxcoord.right = vx+vw;
            out->vtxcoord.bottom = vy+vh;
        }
    }
}

static Result bcfnt_read_file(CFNT_s **out, const char *path)
{
    FILE    *file;
    void    *data;
    Result  res = 0;
    CFNT_s  header = {0};
    file = fopen(path, "rb");
    if (file == NULL)
        return MAKERESULT(RL_USAGE, RS_NOTFOUND, RM_APPLICATION, RD_NOT_FOUND);
    if (fread((void *)&header, 1, sizeof(CFNT_s), file) != sizeof(CFNT_s))
    {
        res = MAKERESULT(RL_USAGE, RS_WRONGARG, RM_APPLICATION, RD_INVALID_SIZE);
        goto exit;
    }
    if (header.signature != 0x554E4643 && header.signature != 0x544E4643) /// CFNU && CFNT
    {
        res = MAKERESULT(RL_USAGE, RS_WRONGARG, RM_APPLICATION, RD_INVALID_SIZE);
        goto exit;
    }
    data = linearAlloc(header.fileSize);
    if (data == NULL)
    {
        res = MAKERESULT(RL_USAGE, RS_OUTOFRESOURCE, RM_APPLICATION, RD_OUT_OF_MEMORY);
        goto exit;
    }
    svcInvalidateProcessDataCache(0xFFFF8001, data, header.fileSize);
    
    fseek(file, 0, SEEK_SET);
    if (fread(data, 1, header.fileSize, file) != header.fileSize)
    {
        free(data);
        res = MAKERESULT(RL_USAGE, RS_WRONGARG, RM_APPLICATION, RD_INVALID_SIZE);
        goto exit;
    }
    *out = (CFNT_s *)data;
    svcFlushProcessDataCache(0xFFFF8001, data, header.fileSize);
exit:
    fclose(file);
    return res;
}

static void bcfnt_relocate_address(void *addr, const u32 base)
{
    *(u32 *)addr += base;
}

static void bcfnt_relocate_CMAP(CMAP_s *node, const u32 base)
{
    if (node == NULL || node->next == NULL)
        return;

    bcfnt_relocate_address(&node->next, base);

    bcfnt_relocate_CMAP(node->next, base);
}

static void bcfnt_relocate_CWDH(CWDH_s *node, const u32 base)
{
    if (node == NULL || node->next == NULL)
        return;

    bcfnt_relocate_address(&node->next, base);

    bcfnt_relocate_CWDH(node->next, base);
}

static Result bcfnt_create_glyphsheet(BCFNT_s *font)
{
    if (font == NULL)
        return MAKERESULT(RL_USAGE, RS_INVALIDARG, RM_APPLICATION, RD_INVALID_POINTER);

    int     i;
    TGLP_s  *glyphInfo = font->cfnt->finf.tglp;
    C3D_Tex *glyphSheets = malloc(sizeof(C3D_Tex)*glyphInfo->nSheets);

    if (glyphSheets == NULL)
        return MAKERESULT(RL_USAGE, RS_OUTOFRESOURCE, RM_APPLICATION, RD_OUT_OF_MEMORY);

    for (i = 0; i < glyphInfo->nSheets; i ++)
    {
        C3D_Tex *tex = &glyphSheets[i];

        tex->data = &glyphInfo->sheetData[i * glyphInfo->sheetSize];
        tex->fmt = glyphInfo->sheetFmt;
        tex->size = glyphInfo->sheetSize;
        tex->width = glyphInfo->sheetWidth;
        tex->height = glyphInfo->sheetHeight;
        tex->param = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR)
            | GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_EDGE) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_EDGE);
        
        //#if ROSALINA
        tex->border = 0;
        tex->lodParam = 0;
        //#endif
    }

    font->glyphSheets = glyphSheets;
    return 0;
}

Result  bcfnt_load_font(BCFNT_s *out, const char *path)
{
    u32             base;
    Result          res = 0;
    FINF_s          *finf;
    charWidthInfo_s *cwi;
    const BCFNT_s   *bak = g_font;
    if (out == NULL || path == NULL)
        return MAKERESULT(RL_USAGE, RS_INVALIDARG, RM_APPLICATION, RD_INVALID_POINTER);
    if (R_FAILED((res = bcfnt_read_file(&out->cfnt, path))))
        return res;
    finf = &out->cfnt->finf;
    base = (u32)out->cfnt;
    bcfnt_relocate_address(&finf->tglp, base);
    bcfnt_relocate_address(&finf->cwdh, base);
    bcfnt_relocate_address(&finf->cmap, base);
    bcfnt_relocate_address(&finf->tglp->sheetData, base);
    bcfnt_relocate_CMAP(finf->cmap, base);
    bcfnt_relocate_CWDH(finf->cwdh, base);
    bcfnt_create_glyphsheet(out);
    out->charPerSheet = finf->tglp->nRows * finf->tglp->nLines;
    g_font = out;
    cwi = pp2d_fontGetCharWidthInfo(pp2d_fontGlyphIndexFromCodePoint(0x3042));
    out->textScale = 20.0f / (cwi->glyphWidth); // 20 is glyphWidth in J machines
    g_font = bak;
    out->cfnt->signature = 0x554E4643; ///< CFNU
    return 0;
}

Result bcfnt_unload_font(BCFNT_s *font)
{
    if (font == NULL || font->cfnt == NULL || font->cfnt->signature != 0x554E4643) ///< CFNU
        return MAKERESULT(RL_USAGE, RS_INVALIDARG, RM_APPLICATION, RD_INVALID_POINTER);

    free(font->glyphSheets);
    font->glyphSheets = NULL;

    linearFree((void *)font->cfnt);
    font->cfnt = NULL;

    return 0;
}
