#include <3ds/types.h>
extern "C" {
#include <3ds/synchronization.h>
}
#include <citro3d.h>

struct C2D_Glyph
{
    u32     lineNo{0};
    C3D_Tex *sheet{nullptr};
    float   xPos{0.f};
    float   width{0.f};
    struct
    {
        float left{0.f}, top{0.f}, right{0.f}, bottom{0.f};
    } texcoord;
};

#include "FontInternal.hpp"

static FT_Library   g_library = nullptr;
static LightLock    g_ftLock;

using namespace CTRFontImpl;

// Small Lock helper
struct Lock
{
    Lock(LightLock &lock) : _lock(lock)
    {
        LightLock_Lock(&_lock);
    }

    ~Lock(void)
    {
        LightLock_Unlock(&_lock);
    }

    LightLock &     _lock;
};

static inline float     PosToFloat(FT_Pos pos) ///< FT_Pos is a 26.6 fixed-point value
{
    return static_cast<float>(pos) / static_cast<float>(1 << 6);
}

static inline float     FixedToFloat(FT_Fixed pos) ///< FT_Fixed is a 16.16 fixed-point value
{
    return static_cast<float>(pos) / static_cast<float>(1 << 16);
}

static inline float     Normalize(float v, float l)
{
    return v / l;
}

static inline int   EnsureFreetypeIsInit(void)
{
    if (g_library == nullptr)
    {
        LightLock_Init(&g_ftLock);
        return FT_Init_FreeType(&g_library);
    }
    return 0;
}

/*
** Sheet
*/

const float     FTfnt::Sheet::width = 256.f;
const float     FTfnt::Sheet::height = 128.f;

FTfnt::Sheet::Sheet(void)
{
    // Clear rows
    for (u32 i = 0; i < sizeof(rows) / sizeof(float); ++i)
        rows[i] = 0.f;

    // Create texture
    tex = new C3D_Tex;
    if (!C3D_TexInit(tex, (u16)width, (u16)height, GPU_A8))
        svcBreak(USERBREAK_PANIC); ///< TODO: better handling ?

    // Clear the texture
    GX_MemoryFill((u32 *)tex->data, 0, (u32 *)((u8 *)tex->data + tex->size), BIT(0) | (tex->fmt << 8), nullptr, 0, nullptr, 0);

    // Define texture antialiasing
    tex->param = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR)
    | GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_EDGE) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_EDGE);
}

FTfnt::Sheet::~Sheet(void)
{
    C3D_TexDelete(tex);
    delete tex;
}

int    FTfnt::Sheet::NewGlyphPosition(const float glWidth)
{
    for (u32 i = 0; i < sizeof(rows) / sizeof(float); ++i)
    {
        float posX = rows[i];

        if (posX + glWidth < width)
            return i;
    }

    return -1;
}

/*
** FTFace
*/
FTfnt::FTFace::FTFace(const std::string &path)
{
    // Init freetype
    if ((error = EnsureFreetypeIsInit()) != 0)
        return;

    // Lock freetype
    Lock    lock(g_ftLock);

    if ((error = FT_New_Face(g_library, path.c_str(), 0, &face)) != 0)
        return;

    // Select unicode charmap
    if ((error = FT_Select_Charmap(face, FT_ENCODING_UNICODE)) != 0)
        return;

    // Force glyph size to 32px*32px max
    if (IsScalable())
    {
        u32 obj = 32;
        u32 size = obj;
        while (error == 0 && size > 10)
        {
            error = FT_Set_Pixel_Sizes(face, 0, size--);
            if (static_cast<u32>(PosToFloat(face->size->metrics.height)) <= obj)
                break;
        }
    }
}

FTfnt::FTFace::~FTFace(void)
{
    FT_Done_Face(face);
}

bool    FTfnt::FTFace::IsScalable(void) { return face->face_flags & FT_FACE_FLAG_SCALABLE; }
float   FTfnt::FTFace::GetCellHeight(void) { return PosToFloat(face->size->metrics.height); }
float   FTfnt::FTFace::GetLineFeed(void) { return PosToFloat(face->size->metrics.height); }
float   FTfnt::FTFace::GetBaseLinePos(void) { return PosToFloat(face->size->metrics.ascender); }
float   FTfnt::FTFace::GetUnderlinePos(void) { return static_cast<float>(face->underline_position); }
int     FTfnt::FTFace::LoadGlyph(u32 code)
{
    u32 glyphIndex = FT_Get_Char_Index(face, code);

    // Load glyph image into the slot (erase previous one)
    if ((error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_NO_AUTOHINT)) != 0)
            return error;

    // Convert to an anti-aliased bitmap
    if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
        error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

    return error;
}

/*
** FTfnt
*/

// From citra
static inline u32   MortonInterleave(u32 posX, u32 posY)
{
    u32 i = (posX & 7) | ((posY & 7) << 8); // ---- -210
    i = (i ^ (i << 2)) & 0x1313;      // ---2 --10
    i = (i ^ (i << 1)) & 0x1515;      // ---2 -1-0
    i = (i | (i >> 7)) & 0x3F;

    return i;
}

static inline u32   GetMortonOffset(u32 posX, u32 posY)
{
    return MortonInterleave(posX, posY) + (posX & ~7) * 8;
}

static void    TextureUpdate(C3D_Tex* tex, const u8* bitmap, u32 posX, u32 posY, u32 width, u32 height)
{
    u8 *texData = (u8 *)tex->data;

    for (u32 y = 0; y < height; ++y)
    {
        for (u32 x = 0; x < width; ++x)
        {
            u32 texY = tex->height - 1 - y - posY;
            u32 texPos = GetMortonOffset(posX + x, texY) + (texY & ~7) * tex->width;

            texData[texPos] = bitmap[x + y * width];
        }
    }

    C3D_TexFlush(tex);
}

/*
** FTfnt
*/

FTfnt::FTfnt(const std::string &path)
    : face(path)
{
    // Init lock
    LightLock_Init(&lock);

    if (face.error)
        return;

    // Init font vars
    lineFeed = face.GetLineFeed();
    //cellHeight = face.GetCellHeight();
    baselinePos = face.GetBaseLinePos();
    underlinePos = face.GetUnderlinePos();

    // Init first texture
    sheets.push_back(new Sheet());
}

FTfnt::~FTfnt(void)
{
    for (Sheet *sheet : sheets)
        delete sheet;
}

using Sheet = FTfnt::Sheet;

Sheet&      FTfnt::GetNewGlyphPosition(int& rowIdx, const float glyphWidth)
{
    for (Sheet *sheet : sheets)
        if ((rowIdx = sheet->NewGlyphPosition(glyphWidth)) >= 0)
            return *sheet;

    // No place available in current sheets, so create a new one
    sheets.push_back(new Sheet);

    return GetNewGlyphPosition(rowIdx, glyphWidth);
}

C2D_Glyph   FTfnt::GetGlyph(const u32 code, float& xAdvance)
{
    if (face.error)
        return C2D_Glyph();

    Lock    _lock(lock);

    // Search in map
    {
        auto pair = glyphs.find(code);

        if (pair != glyphs.end())
        {
            Glyph &    glyph = pair->second;

            xAdvance = glyph.xAdvance;

            return glyph;
        }
    }

    // Load glyph from font
    Lock llock(g_ftLock);

    // Get the bitmap
    if (face.LoadGlyph(code))
        return C2D_Glyph();

    FT_Bitmap&          bitmap = face.face->glyph->bitmap;
    FT_Size_Metrics&    smetrics = face.face->size->metrics;
    FT_Glyph_Metrics&   metrics = face.face->glyph->metrics;

    // Get the sheet which will hold the glyph
    int         rowIdx;
    Sheet &     sheet = GetNewGlyphPosition(rowIdx, bitmap.width);

    // Get pos of new glyph in sheet.tex
    float   texX = sheet.rows[rowIdx];
    float   texY = rowIdx * cellHeight
                 + PosToFloat(smetrics.ascender) ///< Go to baseline
                 - PosToFloat(metrics.horiBearingY); ///< Adjust glyph position

    // Don't waste texture space on empty glyphs
    if (bitmap.width > 0)
    {
        // Write the new glyph data
        TextureUpdate(sheet.tex, (u8 *)bitmap.buffer, (u32)texX, (u32)texY, bitmap.width, bitmap.rows);

        // Update row posX in sheet
        sheet.rows[rowIdx] += (float)bitmap.width + 2.f; ///< 2.f == padding
    }

    // Create the glyph
    Glyph           glyph;
    float           texWidth = Sheet::width;
    float           texHeight = Sheet::height;

    // Glyph metrics
    glyph.xOffset = PosToFloat(metrics.horiBearingX);
    glyph.xAdvance = PosToFloat(metrics.horiAdvance);
    glyph.width = PosToFloat(metrics.width);

    if (bitmap.width > 0)
    {
        glyph.sheet = sheet.tex;

        // Set textures coordinates
        glyph.texcoord.left = Normalize(texX, texWidth);
        glyph.texcoord.top =  Normalize(rowIdx * cellHeight, texHeight);
        glyph.texcoord.right = glyph.texcoord.left + Normalize(bitmap.width, texWidth);
        glyph.texcoord.bottom = glyph.texcoord.top + Normalize(cellHeight - 1.f, texHeight);
    }
    else
    {
        glyph.sheet = nullptr;
        glyph.texcoord.left = glyph.texcoord.top = glyph.texcoord.right = glyph.texcoord.bottom = 0.f;
    }

    // Store the glyph
    glyphs.insert(std::make_pair(code, glyph));

    xAdvance = glyph.xAdvance;

    return glyph;
}
