#pragma once

#include <3ds/types.h>
#include <citro2d.h>
#include <memory>
#include <vector>

#include "Color.hpp"
#include "Font.hpp"

class Text
{
public:
    // Set the font to use for all Text objects without a defined Font (default sysfont)
    static void     SetDefaultFont(const FontHandle &font);

    // Parse a text and cut it correctly (word cut) to fit in defined parameters
    //static Text     CreateFromText(const std::string &src, u32 maxWidth, float scaleX);

    Text(const std::string &src);
    Text(const std::string &src, const FontHandle &font);
    Text(const Color &color, const std::string &src);
    Text(const Color &color, const std::string &src, const FontHandle &font);
    Text(const std::string &src, float scaleX, float scaleY);
    Text(const Color &color, const std::string &src, float scaleX, float scaleY);
    Text(const Color &color, const std::string &src, float scaleX, float scaleY, const FontHandle &font);
    virtual ~Text();

    // Get the nb of glyphs
    u32     GetCount(void) const;
    /* Get the width of the text (depends on SetSize).
    ** In case of multiple lines, returns the biggest width of a line */
    virtual float   GetWidth(void) const;
    // Get the height of the text (depends on SetSize)
    virtual float   GetHeight(void) const;
    // Set the size to be used for Draw or GetWidth
    virtual Text&   SetSize(float scaleX, float scaleY);
    // Set the position to be used for Draw
    virtual Text&   SetPos(float posX, float posY);
    // Set the color to use for drawing the Text
    virtual Text&   SetColor(const Color &color);

    float     Draw(bool atBaseline = false) const;
    float     Draw(float posX, float posY, bool atBaseline = false) const;

private:

    static FontHandle               _defaultFont;

protected:

    u16                             _lines{0};
    u16                             _reserved;
    float                           _posX{0.f};
    float                           _posY{0.f};
    float                           _scaleX{1.f};
    float                           _scaleY{1.f};
    float                           _width{0.f};
    Color                           _color{ Color::Black };
    FontHandle                      _font;
    std::vector<C2D_Glyph>          _glyphs;
};
