#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <memory>
#include <map>
#include <vector>

struct C2D_Glyph
{
    u32     lineNo{0};
    C3D_Tex *sheet{nullptr};
    float   xPos{0.f};
    float   width{0.f};
    struct
    {
        float left, top, right, bottom;
    } texcoord{0.f};
};

class Font;
using FontHandle = std::shared_ptr<Font>;

class Font
{
    public:

        static FontHandle       GetSysfont(void);
        static FontHandle       Open(const std::string &path);

        ~Font();

        // Return 0 if the font is loaded, an error code otherwise
        int         IsLoaded(void) const;
        float       GetCellHeight(void) const;
        float       GetLineFeed(void) const;
        float       GetBaselinePos(void) const;
        float       GetTextScale(void) const;
        C2D_Glyph   GetGlyph(const u32 code, float& xAdvance) const;

    private:

        Font(void *bcfnt);
        // Force the usage of FontHandle
        Font(const std::string &path);

        static FontHandle       _sysfont;

        float                   _textScale{1.f};
        void  *                 _bcfnt{nullptr};
        void  *                 _ftfnt{nullptr};
};
