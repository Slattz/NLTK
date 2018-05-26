#include "CTRFont/Color.hpp"

// From https://www.rapidtables.com
const Color     Color::Black;
const Color     Color::White(255, 255, 255);
const Color     Color::Red(255, 0, 0);
const Color     Color::Lime(0, 255, 0);
const Color     Color::Blue(0, 0, 255);
const Color     Color::Yellow(255, 255, 0);
const Color     Color::Cyan(0, 255, 255);
const Color     Color::Magenta(255, 0, 255);
const Color     Color::Silver(192, 192, 192);
const Color     Color::Gray(128, 128, 128);
const Color     Color::Maroon(128, 0, 0);
const Color     Color::Olive(128, 128, 0);
const Color     Color::Green(0, 128, 0);
const Color     Color::Purple(128, 0, 128);
const Color     Color::Teal(0, 128, 128);
const Color     Color::Navy(0, 0, 128);

Color::Color(void) : raw{0xFF000000}
{
}

Color::Color(u32 raw) : raw{ raw }
{
}

Color::Color(u8 r, u8 g, u8 b, u8 a) :
    r{r}, g{g}, b{b}, a{a}
{
}
