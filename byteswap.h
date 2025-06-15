
/**
 * @brief Hacked version of byteswap.h for systems that lack it
 *
 */
#ifndef BYTESWAP_H
#define BYTESWAP_H

#if defined(__GNUC__)
#define bswap_16 __builtin_bswap16
#define bswap_32 __builtin_bswap32
#define bswap_64 __builtin_bswap64
#else
static inline uint16_t bswap_16(uint16_t x) { return ((((x) & 0x00FF) << 8) | (((x) & 0xFF00) >> 8)); }

static inline uint32_t bswap_32(uint32_t x) {
    return ((x & 0xFF000000) >> 24) | ((x & 0x00FF0000) >> 8) | ((x & 0x0000FF00) << 8) | ((x & 0x000000FF) << 24);
}

static inline uint64_t bswap_64(uint64_t x) {
    return ((((x) & 0x00000000000000FFULL) << 56) | (((x) & 0x000000000000FF00ULL) << 40) |
            (((x) & 0x0000000000FF0000ULL) << 24) | (((x) & 0x00000000FF000000ULL) << 8) |
            (((x) & 0x000000FF00000000ULL) >> 8) | (((x) & 0x0000FF0000000000ULL) >> 24) |
            (((x) & 0x00FF000000000000ULL) >> 40) | (((x) & 0xFF00000000000000ULL) >> 56));
}
#endif

#endif /* BYTESWAP_H */
