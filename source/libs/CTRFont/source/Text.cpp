#include "CTRFont/Text.hpp"
#include <3ds/util/utf.h>
#include "Citro2dInternal.h"
#include <algorithm>

FontHandle      Text::_defaultFont;

using GlyphVector = std::vector<C2D_Glyph>;

void    Text::SetDefaultFont(const FontHandle &font)
{
    _defaultFont = font;
}

static GlyphVector  GetGlyphsFromString(const FontHandle &font, const std::string &src, float &outWidth, u16 &outLines)
{
    u32         code;
    u32         line = 0;
    float       width = 0.f;
    ssize_t     units;
    const u8 *  str = reinterpret_cast<const u8 *>(src.data());
    GlyphVector glyphs;

    if (src.empty())
        return glyphs;

    do
    {
        units = decode_utf8(&code, str);

        // If the character isn't recognized, use the replacement character
        if (units == -1)
        {
            code = 0xFFFD;
            units = 1;
        }

        // If we're at the end of the text, exit
        else if (code == 0)
            break;

        if (code == '\n')
        {
            ++line;
            ++str;

            if (width > outWidth)
                outWidth = width;

            width = 0.f;
            continue;
        }

        str += units;

        float xAdvance;
        C2D_Glyph &&glyph = font->GetGlyph(code, xAdvance);

        if (glyph.width > 0.f)
        {
            glyph.lineNo = line;
            glyph.xPos += width;
            glyphs.push_back(glyph);
        }

        width += xAdvance;

    } while (true);

    // Optimize the glyphs
    std::sort(glyphs.begin(), glyphs.end(),
        [](const C2D_Glyph &g1, const C2D_Glyph &g2)
        {
            return (int)g1.sheet - (int)g2.sheet < 0;
        }
    );

    if (width > outWidth)
        outWidth = width;

    outLines = line + 1;

    return glyphs;
}

Text::Text(const std::string &src)
{
    if (!_defaultFont)
        _defaultFont = Font::GetSysfont();

    _font = _defaultFont;
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
}

Text::Text(const std::string &src, const FontHandle &font) :
    _font{font}
{
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
}

Text::Text(const Color &color, const std::string &src) :
    _color{color}
{
    if (!_defaultFont)
        _defaultFont = Font::GetSysfont();

    _font = _defaultFont;
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
}

Text::Text(const Color &color, const std::string &src, const FontHandle &font) :
    _color{color}, _font{font}
{
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
}

Text::Text(const std::string &src, float scaleX, float scaleY) :
    _scaleX{scaleX}, _scaleY{scaleY}
{
    if (!_defaultFont)
        _defaultFont = Font::GetSysfont();

    _font = _defaultFont;
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
}

Text::Text(const Color &color, const std::string &src, float scaleX, float scaleY) :
    _scaleX{scaleX}, _scaleY{scaleY}, _color{color}
{
    if (!_defaultFont)
        _defaultFont = Font::GetSysfont();

    _font = _defaultFont;
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
}

Text::Text(const Color &color, const std::string &src, float scaleX, float scaleY, const FontHandle &font) :
    _scaleX{scaleX}, _scaleY{scaleY}, _color{color}, _font{font}
{
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
}

Text::~Text(void)
{
    // Nothing to do, default dtors do the job
}

u32     Text::GetCount(void) const
{
    return _glyphs.size();
}

float   Text::GetWidth(void) const
{
    return _width * _scaleX;
}

float   Text::GetHeight(void) const
{
    return (float)_lines * (_scaleY * _font->GetLineFeed());
}

Text&   Text::SetSize(float scaleX, float scaleY)
{
    scaleX *= _font->GetTextScale();
    scaleY *= _font->GetTextScale();

    _scaleX = scaleX;
    _scaleY = scaleY;

    return *this;
}

Text&   Text::SetPos(float posX, float posY)
{
    _posX = posX;
    _posY = posY;

    return *this;
}

Text&   Text::SetColor(const Color &color)
{
    _color = color;

    return *this;
}

float     Text::Draw(bool atBaseline) const
{
    float   glyphH = _scaleY * _font->GetCellHeight();
    float   dispY = _scaleY * _font->GetLineFeed();
    float   blend = 1.f;
    float   glyphZ = 0.5f;
    float   posY = _posY;
    u32     line = 1;

    if (atBaseline)
        posY -= _scaleY * _font->GetBaselinePos();

    for (const C2D_Glyph &glyph : _glyphs)
    {
        float   glyphW = _scaleX * glyph.width;
        float   glyphX = _posX + _scaleX * glyph.xPos;
        float   glyphY = posY + dispY * glyph.lineNo;

        line = std::max(line, glyph.lineNo);

        C2Di_SetTex(glyph.sheet);
        C2Di_Update();
        C2Di_AppendVtx(glyphX,        glyphY,        glyphZ, glyph.texcoord.left,  glyph.texcoord.top,    blend, _color.raw);
        C2Di_AppendVtx(glyphX,        glyphY+glyphH, glyphZ, glyph.texcoord.left,  glyph.texcoord.bottom, blend, _color.raw);
        C2Di_AppendVtx(glyphX+glyphW, glyphY,        glyphZ, glyph.texcoord.right, glyph.texcoord.top,    blend, _color.raw);
        C2Di_AppendVtx(glyphX+glyphW, glyphY,        glyphZ, glyph.texcoord.right, glyph.texcoord.top,    blend, _color.raw);
        C2Di_AppendVtx(glyphX,        glyphY+glyphH, glyphZ, glyph.texcoord.left,  glyph.texcoord.bottom, blend, _color.raw);
        C2Di_AppendVtx(glyphX+glyphW, glyphY+glyphH, glyphZ, glyph.texcoord.right, glyph.texcoord.bottom, blend, _color.raw);
    }

    return posY + dispY * line;
}

float     Text::Draw(float posX, float posY, bool atBaseline) const
{
    float   glyphH = _scaleY * _font->GetCellHeight();
    float   dispY = _scaleY * _font->GetLineFeed();
    float   blend = 1.f;
    float   glyphZ = 0.5f;
    float   posYY = posY;
    u32     line = 1;

    if (atBaseline)
        posYY -= _scaleY * _font->GetBaselinePos();

    for (const C2D_Glyph &glyph : _glyphs)
    {
        float   glyphW = _scaleX * glyph.width;
        float   glyphX = posX + _scaleX * glyph.xPos;
        float   glyphY = posYY + dispY * glyph.lineNo;

        line = std::max(line, glyph.lineNo);

        C2Di_SetTex(glyph.sheet);
        C2Di_Update();
        C2Di_AppendVtx(glyphX,        glyphY,        glyphZ, glyph.texcoord.left,  glyph.texcoord.top,    blend, _color.raw);
        C2Di_AppendVtx(glyphX,        glyphY+glyphH, glyphZ, glyph.texcoord.left,  glyph.texcoord.bottom, blend, _color.raw);
        C2Di_AppendVtx(glyphX+glyphW, glyphY,        glyphZ, glyph.texcoord.right, glyph.texcoord.top,    blend, _color.raw);
        C2Di_AppendVtx(glyphX+glyphW, glyphY,        glyphZ, glyph.texcoord.right, glyph.texcoord.top,    blend, _color.raw);
        C2Di_AppendVtx(glyphX,        glyphY+glyphH, glyphZ, glyph.texcoord.left,  glyph.texcoord.bottom, blend, _color.raw);
        C2Di_AppendVtx(glyphX+glyphW, glyphY+glyphH, glyphZ, glyph.texcoord.right, glyph.texcoord.bottom, blend, _color.raw);
    }

    return posY + dispY * line;
}
