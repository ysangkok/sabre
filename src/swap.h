#include <inttypes.h>

static inline uint16_t ltohs(uint16_t x)
{
    /* This is compile-time optimised with at least -O1 or -Os */
    uint32_t const dummy = 0x12345678;
    if(*(uint8_t const *)&dummy == 0x12)
        return (uint16_t) ((x >> 8) | (x << 8));
    else
        return x;
}

static inline uint32_t ltohl(uint32_t x)
{
    /* This is compile-time optimised with at least -O1 or -Os */
    uint32_t const dummy = 0x12345678;
    if(*(uint8_t const *)&dummy == 0x12)
        return (x >> 24) | ((x >> 8) & 0x0000ff00)
                | ((x << 8) & 0x00ff0000) | (x << 24);
    else
        return x;
}

