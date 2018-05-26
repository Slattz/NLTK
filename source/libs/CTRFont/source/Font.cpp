#include "CTRFont/Font.hpp"
#include "FontInternal.hpp"

extern "C"
CFNT_s *    g_sharedFont;   ///< defined in ctrulib
FontHandle  Font::_sysfont;

using CTRFontImpl::Bcfnt;
using CTRFontImpl::FTfnt;

FontHandle  Font::GetSysfont(void)
{
    if (!_sysfont)
    {
        // Ensure the shared system font is mapped
        if (R_FAILED(fontEnsureMapped()))
            svcBreak(USERBREAK_PANIC);

        Bcfnt * bcfnt = new Bcfnt;
        Font  * font = new Font(static_cast<void *>(bcfnt));

        bcfnt->cfnt = g_sharedFont;
        bcfnt->charPerSheet = bcfnt->GetGlyphInfo()->nRows * bcfnt->GetGlyphInfo()->nLines;
        bcfnt->CreateGlyphsheet();
        font->_textScale = 30.f / bcfnt->GetGlyphInfo()->cellHeight;

        _sysfont = FontHandle(font);
    }

    return _sysfont;
}

FontHandle  Font::Open(const std::string &path)
{
    return FontHandle(new Font(path));
}

Font::Font(void *bcfnt) : _bcfnt{bcfnt}
{
}

Font::Font(const std::string &path)
{
    std::size_t     pos = path.rfind(".");

    if (pos != std::string::npos)
    {
        std::string extension = path.substr(pos + 1);

        // Handle custom extensions here
        if (extension == "bcfnt")
        {
            FILE  * file = fopen(path.c_str(), "rb");
            Bcfnt * bcfnt = new Bcfnt;

            if (bcfnt->LoadFromFile(file) == 0)
            {
                _bcfnt = static_cast<void *>(bcfnt);
                _textScale = 20.f / bcfnt->GetCharWidthInfo(bcfnt->GlyphIndexFromCodePoint(0x3042)).glyphWidth;

                return;
            }
        }
    }

    // Try to load the font with freetype
    _ftfnt = static_cast<void *>(new FTfnt(path));
}

Font::~Font(void)
{
    if (_bcfnt != nullptr)
    {
        Bcfnt *bcfnt = reinterpret_cast<Bcfnt *>(_bcfnt);

        if (bcfnt->glyphSheets)
        {
            delete[] bcfnt->glyphSheets;
            bcfnt->glyphSheets = nullptr;
        }

        if (bcfnt->cfnt && bcfnt->cfnt != g_sharedFont)
        {
            linearFree(bcfnt->cfnt);
            bcfnt->cfnt = nullptr;
        }

        delete reinterpret_cast<Bcfnt *>(_bcfnt);
    }
    if (_ftfnt != nullptr)
    {
        delete reinterpret_cast<FTfnt *>(_ftfnt);
    }
}

int     Font::IsLoaded(void) const
{
    if (_bcfnt != nullptr)
        return 0;
    if (_ftfnt  != nullptr)
        return reinterpret_cast<FTfnt *>(_ftfnt)->face.error;
    return -1;
}

float   Font::GetCellHeight(void) const
{
    if (_bcfnt != nullptr)
        return reinterpret_cast<Bcfnt *>(_bcfnt)->GetGlyphInfo()->cellHeight;
    if (_ftfnt != nullptr)
        return reinterpret_cast<FTfnt *>(_ftfnt)->cellHeight;

    return 0.f;
}

float   Font::GetLineFeed(void) const
{
    if (_bcfnt != nullptr)
        return reinterpret_cast<Bcfnt *>(_bcfnt)->GetInfo()->lineFeed;
    if (_ftfnt != nullptr)
        return reinterpret_cast<FTfnt *>(_ftfnt)->lineFeed;

    return 0.f;
}

float   Font::GetBaselinePos(void) const
{
    if (_bcfnt != nullptr)
        return reinterpret_cast<Bcfnt *>(_bcfnt)->GetGlyphInfo()->baselinePos;
    if (_ftfnt != nullptr)
        return reinterpret_cast<FTfnt *>(_ftfnt)->baselinePos;

    return 0.f;
}

float   Font::GetTextScale(void) const
{
    return _textScale;
}

C2D_Glyph   Font::GetGlyph(const u32 code, float& xAdvance) const
{
    C2D_Glyph   glyph;

    if (_bcfnt != nullptr)
    {
        Bcfnt *bcfnt = reinterpret_cast<Bcfnt *>(_bcfnt);
        xAdvance = bcfnt->CalcGlyphPos(glyph, bcfnt->GlyphIndexFromCodePoint(code), bcfnt->glyphSheets);

        return glyph;
    }

    if (_ftfnt != nullptr)
        return reinterpret_cast<FTfnt *>(_ftfnt)->GetGlyph(code, xAdvance);

    // Error, return an empty glyph
    return glyph;
}
