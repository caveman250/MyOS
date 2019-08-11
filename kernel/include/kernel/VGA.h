#ifndef ARCH_I386_VGA_H
#define ARCH_I386_VGA_H

#include <stdint.h>

namespace kernel
{
    class VGA
    {
    public:
        enum Colour {
            COLOUR_BLACK = 0,
            COLOUR_BLUE = 1,
            COLOUR_GREEN = 2,
            COLOUR_CYAN = 3,
            COLOUR_RED = 4,
            COLOUR_MAGENTA = 5,
            COLOUR_BROWN = 6,
            COLOUR_LIGHT_GREY = 7,
            COLOUR_DARK_GREY = 8,
            COLOUR_LIGHT_BLUE = 9,
            COLOUR_LIGHT_GREEN = 10,
            COLOUR_LIGHT_CYAN = 11,
            COLOUR_LIGHT_RED = 12,
            COLOUR_LIGHT_MAGENTA = 13,
            COLOUR_LIGHT_BROWN = 14,
            COLOUR_WHITE = 15,
        };

        static inline uint8_t CreateColour(enum Colour fg, enum Colour bg) 
        {
            return fg | bg << 4;
        }

        static inline uint16_t Entry(unsigned char uc, uint8_t colour) 
        {
            return (uint16_t) uc | (uint16_t) colour << 8;
        }
    };
}
#endif