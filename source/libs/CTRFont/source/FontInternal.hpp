#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>
#include <vector>

namespace CTRFontImpl
{
    struct  Bcfnt
    {
        CFNT_s  *           cfnt{nullptr};
        C3D_Tex *           glyphSheets{nullptr};
        int                 charPerSheet{0};

        int                 CreateGlyphsheet(void);
        int                 LoadFromFile(FILE *file);
        FINF_s *            GetInfo(void) const;
        TGLP_s *            GetGlyphInfo(void) const;
        void   *            GetGlyphSheetTex(int sheetIndex) const;
        int                 GlyphIndexFromCodePoint(u32 codePoint) const;
        charWidthInfo_s &   GetCharWidthInfo(int glyphIndex) const;
        float               CalcGlyphPos(C2D_Glyph & out, int glyphIndex, C3D_Tex *glyphSheets) const;
    };

    struct FTfnt
    {
        struct Glyph ///< Used internally
        {
            float       xOffset;
            float       xAdvance;
            float       width;
            C3D_Tex *   sheet;
            struct
            {
                float left, top, right, bottom;
            } texcoord;

            operator C2D_Glyph() const
            {
                C2D_Glyph   glyph;

                glyph.sheet = sheet;
                glyph.xPos = xOffset;
                glyph.width = width;
                glyph.texcoord.left = texcoord.left;
                glyph.texcoord.top = texcoord.top;
                glyph.texcoord.right = texcoord.right;
                glyph.texcoord.bottom = texcoord.bottom;

                return glyph;
            }
        };

        struct Sheet
        {
            static const float  width;
            static const float  height;

            u32             refCount{0};
            float           rows[4];
            C3D_Tex *       tex{nullptr}; ///< 256px * 128px

            Sheet(void);
            ~Sheet(void);

            // Return < 0 if the sheet can't hold the new glyph
            int    NewGlyphPosition(const float glWidth);
        };

        struct FTFace
        {
            FT_Face     face;
            FT_Error    error;

            FTFace(const std::string &path);
            ~FTFace(void);

            bool    IsScalable(void);
            float   GetCellHeight(void);
            float   GetLineFeed(void);
            float   GetBaseLinePos(void);
            int     LoadGlyph(const u32 code);
        };

        float                       lineFeed{0.f};
        float                       cellHeight{32.f};
        float                       baselinePos{0.f};
        FTFace                      face;
        LightLock                   lock;
        std::vector<Sheet *>        sheets;
        std::map<const u32, Glyph>  glyphs;

        FTfnt(const std::string &path);
        ~FTfnt(void);

        Sheet &     GetNewGlyphPosition(int& rowIdx, const float glyphWidth);
        C2D_Glyph   GetGlyph(const u32 code, float& xAdvance);
    };

    inline FINF_s * Bcfnt::GetInfo(void) const
    {
        return &cfnt->finf;
    }

    inline TGLP_s * Bcfnt::GetGlyphInfo(void) const
    {
        return GetInfo()->tglp;
    }

    inline void *   Bcfnt::GetGlyphSheetTex(int sheetIndex) const
    {
        TGLP_s * tglp = GetGlyphInfo();
        return &tglp->sheetData[sheetIndex * tglp->sheetSize];
    }
}
