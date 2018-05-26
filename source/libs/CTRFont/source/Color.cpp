#include "Color.hpp"

const Color     Color::Black;
const Color     Color::White(255, 255, 255);
const Color     Color::Red(255, 0, 0);
const Color     Color::Green(0, 128, 0);
const Color     Color::Blue(0, 0, 255);
const Color     Color::Yellow(255, 255, 0);

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
