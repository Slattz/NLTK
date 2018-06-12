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

static GlyphVector  GetGlyphsFromString(const FontHandle &font, const std::string &src, float &outWidth,
                                        u16 &outLines, u32 line = 0, float width = 0.f)
{
    u32         code;
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

        if (code == '\r')
        {
            ++str;
            continue;
        }

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

    if (width > outWidth)
        outWidth = width;

    outLines = line + 1;

    return glyphs;
}

Text::Text(void)
{
    if (!_defaultFont)
        _defaultFont = Font::GetSysfont();

    _font = _defaultFont;
}

Text::Text(const Text &text)
{
    _lines = text._lines;
    _flags = text._flags;
    _bounds.left = text._bounds.left;
    _bounds.top = text._bounds.top;
    _bounds.width = text._bounds.width;
    _bounds.height = text._bounds.height;
    _posX = text._posX;
    _posY = text._posY;
    _scaleX = text._scaleX;
    _scaleY = text._scaleY;
    _width = text._width;
    _color.raw = text._color.raw;
    _outlineColor.raw = text._outlineColor.raw;
    _text = text._text;
    _font = text._font;
    _glyphs = text._glyphs;

    Style.bold = text.Style.bold;
    Style.italic = text.Style.italic;
    Style.underline = text.Style.underline;
    Style.outline = text.Style.outline;
    Style.strikethrough = text.Style.strikethrough;

}

Text::Text(Text &&text)
{
    _lines = text._lines;
    _flags = text._flags;
    _bounds.left = text._bounds.left;
    _bounds.top = text._bounds.top;
    _bounds.width = text._bounds.width;
    _bounds.height = text._bounds.height;
    _posX = text._posX;
    _posY = text._posY;
    _scaleX = text._scaleX;
    _scaleY = text._scaleY;
    _width = text._width;
    _color.raw = text._color.raw;
    _outlineColor.raw = text._outlineColor.raw;
    _text = std::move(text._text);
    _font = text._font;
    _glyphs = std::move(text._glyphs);

    Style.bold = text.Style.bold;
    Style.italic = text.Style.italic;
    Style.underline = text.Style.underline;
    Style.outline = text.Style.outline;
    Style.strikethrough = text.Style.strikethrough;
}

Text::Text(const std::string &src) : _text{src}
{
    if (!_defaultFont)
        _defaultFont = Font::GetSysfont();

    _font = _defaultFont;
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
    _OptimizeGlyphs();
}

Text::Text(const std::string &src, const FontHandle &font) :
    _text{src}, _font{font}
{
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
    _OptimizeGlyphs();
}

Text::Text(const Color &color, const std::string &src) :
    _color{color}, _text{src}
{
    if (!_defaultFont)
        _defaultFont = Font::GetSysfont();

    _font = _defaultFont;
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
    _OptimizeGlyphs();
}

Text::Text(const Color &color, const std::string &src, const FontHandle &font) :
    _color{color}, _text{src}, _font{font}
{
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
    _OptimizeGlyphs();
}

Text::Text(const std::string &src, float scaleX, float scaleY) :
    _scaleX{scaleX}, _scaleY{scaleY}, _text{src}
{
    if (!_defaultFont)
        _defaultFont = Font::GetSysfont();

    _font = _defaultFont;
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
    _OptimizeGlyphs();
}

Text::Text(const Color &color, const std::string &src, float scaleX, float scaleY) :
    _scaleX{scaleX}, _scaleY{scaleY}, _color{color}, _text{src}
{
    if (!_defaultFont)
        _defaultFont = Font::GetSysfont();

    _font = _defaultFont;
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
    _OptimizeGlyphs();
}

Text::Text(const Color &color, const std::string &src, float scaleX, float scaleY,
            const FontHandle &font) :
    _scaleX{scaleX}, _scaleY{scaleY}, _color{color}, _text{src}, _font{font}
{
    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
    _OptimizeGlyphs();
}

Text::Text(const Color &color, const std::string &src, float scaleX, float scaleY,
            float posX, float posY) :
    _posX{posX}, _posY{posY}, _scaleX{scaleX}, _scaleY{scaleY}, _color{color}, _text{src}
{
    if (!_defaultFont)
        _defaultFont = Font::GetSysfont();

    _font = _defaultFont;

    _glyphs = GetGlyphsFromString(_font, src, _width, _lines);
    _OptimizeGlyphs();
}

Text::~Text(void)
{
    // Nothing to do, default dtors do the job
}

void    Text::_OptimizeGlyphs(void)
{
    // Optimize the glyphs
    std::sort(_glyphs.begin(), _glyphs.end(),
        [](const C2D_Glyph &g1, const C2D_Glyph &g2)
        {
            return (int)g1.sheet - (int)g2.sheet < 0;
        }
    );
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
    return static_cast<float>(_lines) * (_scaleY * _font->GetLineFeed());
}

const std::string&   Text::GetText(void) const
{
    return _text;
}

Text&   Text::SetSize(float scaleX, float scaleY)
{
    scaleX *= _font->GetTextScale();
    scaleY *= _font->GetTextScale();

    _scaleX = scaleX;
    _scaleY = scaleY;

    if (_isCentered)
        _CenterInBounds();

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

Text&   Text::SetOutlineColor(const Color &color)
{
    _outlineColor = color;

    return *this;
}

Text&   Text::SetStyle(const u32 style)
{
    Style.bold = style & BOLD;
    Style.italic = style & ITALIC;
    Style.underline = style & UNDERLINE;
    Style.outline = style & OUTLINE;
    Style.strikethrough = style & STRIKETHROUGH;

    return *this;
}

Text&   Text::CenterInBounds(float left, float top, float width, float height)
{
    _isCentered = true;
    _bounds.left = left;
    _bounds.top = top;
    _bounds.width = width;
    _bounds.height = height;

    _CenterInBounds();

    return *this;
}

static void  AppendVtx(C2Di_Context* ctx, float x, float y, float u, float v, u32 color)
{
    C2Di_Vertex* vtx = &ctx->vtxBuf[ctx->vtxBufPos++];
    vtx->pos[0]      = x;
    vtx->pos[1]      = y;
    vtx->pos[2]      = 0.5f;
    vtx->texcoord[0] = u;
    vtx->texcoord[1] = v;
    vtx->blend[0]    = 0.0f;
    vtx->blend[1]    = 1.0f;
    vtx->color       = color;
}

void      Text::_Draw(const u32 color, float posX, float posY, const bool atBaseline, const bool outline) const
{
    C2Di_Context* ctx = C2Di_GetContext();

    float   scalePixY = outline ? 2.f / _font->GetCellHeight() * _scaleY : 0.f;
    float   scaleY = _scaleY + scalePixY;
    float   glyphH = scaleY * _font->GetCellHeight();
    float   dispY = _scaleY * _font->GetLineFeed();
    float   italic = Style.italic ? 0.245f * glyphH : 0.f;
    float   centerY = _font->GetCellHeight() / 2.f;

    if (atBaseline)
        posY -= _scaleY * _font->GetBaselinePos() - outline ? scaleY * _font->GetBaselinePos() : 0.f;
    else
        posY += _scaleY * centerY - scaleY * centerY;

    for (const C2D_Glyph &glyph : _glyphs)
    {
        float   scaleX = _scaleX + (outline ? 2.f / glyph.width * _scaleX : 0.f);
        float   centerX = glyph.width / 2.f;
        float   glyphW = scaleX * glyph.width;
        float   glyphX = posX + _scaleX * glyph.xPos + _scaleX * centerX - scaleX * centerX;
        float   glyphY = posY + dispY * glyph.lineNo;

        float   glyphXW = glyphX + glyphW;
        float   glyphYH = glyphY + glyphH;

        C2Di_SetTex(glyph.sheet);
        C2Di_Update();
        AppendVtx(ctx, glyphX + italic,  glyphY,  glyph.texcoord.left,  glyph.texcoord.top,    color);
        AppendVtx(ctx, glyphX,           glyphYH, glyph.texcoord.left,  glyph.texcoord.bottom, color);
        AppendVtx(ctx, glyphXW + italic, glyphY,  glyph.texcoord.right, glyph.texcoord.top,    color);
        AppendVtx(ctx, glyphXW + italic, glyphY,  glyph.texcoord.right, glyph.texcoord.top,    color);
        AppendVtx(ctx, glyphX,           glyphYH, glyph.texcoord.left,  glyph.texcoord.bottom, color);
        AppendVtx(ctx, glyphXW,          glyphYH, glyph.texcoord.right, glyph.texcoord.bottom, color);
    }
}

float     Text::Draw(bool atBaseline) const
{
    if (_isCentered)
    {
        C2Di_Context* ctx = C2Di_GetContext();

        u32 sceneW = ctx->sceneW;
        u32 sceneH = ctx->sceneH;

        u32 left = sceneH - (u32)(_bounds.top + _bounds.height);
        u32 top = sceneW - (u32)(_bounds.left + _bounds.width);
        u32 right = left + (u32)_bounds.height;
        u32 bottom = top + (u32)_bounds.width;

        C3D_SetScissor(GPU_SCISSOR_NORMAL, left, top, right, bottom);
    }

    if (Style.outline)
        _Draw(_outlineColor.raw, _posX, _posY, atBaseline, true);

    _Draw(_color.raw, _posX, _posY, atBaseline, false);

    if (_isCentered)
    {
        C2Di_FlushVtxBuf();
        C3D_SetScissor(GPU_SCISSOR_DISABLE, 0, 0, 0, 0);
    }

    return _posY + _scaleY * _font->GetLineFeed() * _lines;
}

float     Text::Draw(float posX, float posY, bool atBaseline) const
{
    if (_isCentered)
    {
        C2Di_Context* ctx = C2Di_GetContext();

        u32 sceneW = ctx->sceneW;
        u32 sceneH = ctx->sceneH;

        u32 left = sceneH - (u32)(_bounds.top + _bounds.height);
        u32 top = sceneW - (u32)(_bounds.left + _bounds.width);
        u32 right = left + (u32)_bounds.height;
        u32 bottom = top + (u32)_bounds.width;

        C3D_SetScissor(GPU_SCISSOR_NORMAL, left, top, right, bottom);
    }

    if (Style.outline)
        _Draw(_outlineColor.raw, posX, posY, atBaseline, true);

    _Draw(_color.raw, posX, posY, atBaseline, false);

    if (_isCentered)
    {
        C2Di_FlushVtxBuf();
        C3D_SetScissor(GPU_SCISSOR_DISABLE, 0, 0, 0, 0);
    }

    return posY + _scaleY * _font->GetLineFeed() * _lines;
}

void    Text::_CenterInBounds(void)
{
    // Center current text according to scale
    float   textHeight = GetHeight();
    float   textWidth = GetWidth();

    _posX = _bounds.left + ((_bounds.width - textWidth) / 2.f);
    _posY = _bounds.top + ((_bounds.height - textHeight) / 2.f);
}

/*
** Operators
*/

Text&   Text::operator=(const Text &text)
{
    _lines = text._lines;
    _flags = text._flags;
    _bounds.left = text._bounds.left;
    _bounds.top = text._bounds.top;
    _bounds.width = text._bounds.width;
    _bounds.height = text._bounds.height;
    _posX = text._posX;
    _posY = text._posY;
    _scaleX = text._scaleX;
    _scaleY = text._scaleY;
    _width = text._width;
    _color.raw = text._color.raw;
    _outlineColor.raw = text._outlineColor.raw;
    _text = text._text;
    _font = text._font;
    _glyphs = text._glyphs;

    return *this;
}

Text&   Text::operator=(const std::string &string)
{
    _text = string;
    _glyphs = GetGlyphsFromString(_font, string, _width, _lines);
    _OptimizeGlyphs();

    return *this;
}

Text&   Text::operator+=(const Text &text)
{
    _text += text._text;
    _glyphs = GetGlyphsFromString(_font, _text, _width, _lines);
    _OptimizeGlyphs();

    // Optimize the glyphs
    _OptimizeGlyphs();

    return *this;
}

Text&   Text::operator+=(const std::string &string)
{
    _text += string;
    _glyphs = GetGlyphsFromString(_font, _text, _width, _lines);
    _OptimizeGlyphs();

    // Optimize the glyphs
    _OptimizeGlyphs();

    return *this;
}

Text    Text::operator+(const Text &right) const
{
    Text    ret(*this);

    ret += right;
    return ret;
}

Text    Text::operator+(const std::string &right) const
{
    Text    ret(*this);

    ret += right;
    return ret;
}

Text    operator+(const std::string &left, const Text &right)
{
    Text    text(right._color, left + right._text, right._scaleX, right._scaleY, right._font);

    return text;
}
