#pragma once

#include <3ds/types.h>
#include <memory>
#include <vector>

#include "Color.hpp"
#include "Font.hpp"

enum
{
    BOLD = 1 << 1,
    ITALIC = 1 << 2,
    UNDERLINE = 1 << 3,
    OUTLINE = 1 << 4,
    STRIKETHROUGH = 1 << 5
};

class Text
{
public:
    // Set the font to use for all Text objects without a defined Font (default sysfont)
    static void     SetDefaultFont(const FontHandle &font);

    // Parse a text and cut it correctly to fit in defined parameters
    // static Text     Parse(const std::string &src, float maxWidth, float scaleX);

    Text(void);
    Text(const Text &text);
    Text(Text &&text);
    Text(const std::string &src);
    Text(const std::string &src, const FontHandle &font);
    Text(const Color &color, const std::string &src);
    Text(const Color &color, const std::string &src, const FontHandle &font);
    Text(const std::string &src, float scaleX, float scaleY);
    Text(const Color &color, const std::string &src, float scaleX, float scaleY);
    Text(const Color &color, const std::string &src, float scaleX, float scaleY, const FontHandle &font);
    Text(const Color &color, const std::string &src, float scaleX, float scaleY, float posX, float posY);
    virtual ~Text();

    // Get the nb of glyphs
    u32     GetCount(void) const;
    /* Get the width of the text (depends on SetSize).
    ** In case of multiple lines, returns the biggest width of a line */
    virtual float   GetWidth(void) const;
    // Get the height of the text (depends on SetSize)
    virtual float   GetHeight(void) const;
    // Get a std::string with the text of this object
    const std::string&     GetText(void) const;
    // Set the size to be used for Draw or GetWidth
    virtual Text&   SetSize(float scaleX, float scaleY);
    // Set the position to be used for Draw
    virtual Text&   SetPos(float posX, float posY);
    // Set the color to use for drawing the Text
    virtual Text&   SetColor(const Color &color);
    // Set the color to use for the outline
    virtual Text&   SetOutlineColor(const Color &color);
    // Set style using the enumeration higher
    virtual Text&   SetStyle(const u32 style);

    float     Draw(bool atBaseline = false) const;
    float     Draw(float posX, float posY, bool atBaseline = false) const;

    Text&   operator=(const Text &text);
    Text&   operator=(const std::string &string);
    Text&   operator+=(const Text &text);
    Text&   operator+=(const std::string &string);
    Text    operator+(const Text &right) const;
    Text    operator+(const std::string &right) const;

    struct
    {
        bool    bold : 1;   ///< Not yet supported
        bool    italic : 1;
        bool    underline : 1; ///< Not yet supported
        bool    outline : 1;
        bool    strikethrough : 1; ///< Not yet supported
    }   Style{false, false, false, false, false};

private:

    friend Text     operator+(const std::string &left, const Text &right);

    static FontHandle               _defaultFont;

protected:

    void    _OptimizeGlyphs(void);
    void    _Draw(const u32 color, float posX, float posY, const bool atBaseline, const bool outline) const;

    u16                             _lines{0};
    u16                             _reserved;
    float                           _posX{0.f};
    float                           _posY{0.f};
    float                           _scaleX{1.f};
    float                           _scaleY{1.f};
    float                           _width{0.f};
    Color                           _color{ Color::Black };
    Color                           _outlineColor{ Color::White };
    std::string                     _text;
    FontHandle                      _font;
    std::vector<C2D_Glyph>          _glyphs;
};

Text     operator+(const std::string &left, const Text &right);
