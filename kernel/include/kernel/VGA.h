#pragma once
#include <stdint.h>

namespace kernel
{
    class VGA
    {
    public:
        enum class Colour 
        {
            BLACK = 0,
            BLUE = 1,
            GREEN = 2,
            CYAN = 3,
            RED = 4,
            MAGENTA = 5,
            BROWN = 6,
            LIGHT_GREY = 7,
            DARK_GREY = 8,
            LIGHT_BLUE = 9,
            LIGHT_GREEN = 10,
            LIGHT_CYAN = 11,
            LIGHT_RED = 12,
            LIGHT_MAGENTA = 13,
            LIGHT_BROWN = 14,
            WHITE = 15,
        };

        static inline uint8_t CreateColour(enum Colour fg, enum Colour bg) 
        {
            return (uint8_t)fg | (uint8_t)bg << 4;
        }

        static inline uint16_t Entry(unsigned char uc, uint8_t colour) 
        {
            return (uint16_t) uc | (uint16_t) colour << 8;
        }
    };
}