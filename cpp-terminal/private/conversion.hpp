#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#ifdef _WIN32
#include <stdio.h>
#else
#include <sys/ioctl.h>
#endif

namespace Term::Private {
static constexpr uint8_t UTF8_ACCEPT = 0;
static constexpr uint8_t UTF8_REJECT = 0xf;

inline uint8_t utf8_decode_step(uint8_t state, uint8_t octet, uint32_t* cpp) {
    static const uint32_t utf8_classtab[0x10] = {
        0x88888888UL, 0x88888888UL, 0x99999999UL, 0x99999999UL,
        0xaaaaaaaaUL, 0xaaaaaaaaUL, 0xaaaaaaaaUL, 0xaaaaaaaaUL,
        0x222222ffUL, 0x22222222UL, 0x22222222UL, 0x22222222UL,
        0x3333333bUL, 0x33433333UL, 0xfff5666cUL, 0xffffffffUL,
    };

    static const uint32_t utf8_statetab[0x10] = {
        0xfffffff0UL, 0xffffffffUL, 0xfffffff1UL, 0xfffffff3UL,
        0xfffffff4UL, 0xfffffff7UL, 0xfffffff6UL, 0xffffffffUL,
        0x33f11f0fUL, 0xf3311f0fUL, 0xf33f110fUL, 0xfffffff2UL,
        0xfffffff5UL, 0xffffffffUL, 0xffffffffUL, 0xffffffffUL,
    };

    const uint8_t reject = (state >> 3), nonascii = (octet >> 7);
    const uint8_t class_ =
        (!nonascii ? 0
                   : (0xf & (utf8_classtab[(octet >> 3) & 0xf] >>
                             (4 * (octet & 7)))));

    *cpp = (state == UTF8_ACCEPT ? (octet & (0xffU >> class_))
                                 : ((octet & 0x3fU) | (*cpp << 6)));

    return (reject ? 0xf
                   : (0xf & (utf8_statetab[class_] >> (4 * (state & 7)))));
}

inline void codepoint_to_utf8(std::string& s, char32_t c) {
    if (c > 0x0010FFFF) {
        throw std::runtime_error("Invalid UTF32 codepoint.");
    }
    char bytes[4];
    int nbytes = 1;
    char32_t d = c;
    if (c >= 0x10000) {
        nbytes++;
        bytes[3] = ((d | 0x80) & 0xBF);
        d >>= 6;
    }
    if (c >= 0x800) {
        nbytes++;
        bytes[2] = ((d | 0x80) & 0xBF);
        d >>= 6;
    }
    if (c >= 0x80) {
        nbytes++;
        bytes[1] = ((d | 0x80) & 0xBF);
        d >>= 6;
    }
    static const unsigned char mask[4] = {0x00, 0xC0, 0xE0, 0xF0};
    bytes[0] = static_cast<char>(d | mask[nbytes - 1]);
    s.append(bytes, nbytes);
}

inline std::u32string utf8_to_utf32(const std::string& s) {
    uint32_t codepoint{};
    uint8_t state = UTF8_ACCEPT;
    std::u32string r{};
    for (char i : s) {
        state = utf8_decode_step(state, i, &codepoint);
        if (state == UTF8_ACCEPT) {
            r.push_back(codepoint);
        } else if (state == UTF8_REJECT) {
            throw std::runtime_error("Invalid byte in UTF8 encoded string");
        }
    }
    if (state != UTF8_ACCEPT) {
        throw std::runtime_error("Expected more bytes in UTF8 encoded string");
    }
    return r;
}

inline std::string utf32_to_utf8(const std::u32string& s) {
    std::string r{};
    for (char32_t i : s) {
        codepoint_to_utf8(r, i);
    }
    return r;
}
// coverts a string into an integer
inline int convert_string_to_int(const char* string,
                                 const char* format,
                                 int* rows,
                                 int* cols) {
#ifdef _WIN32
    // windows provides it's own alternative to sscanf()
    return sscanf_s(string, format, rows, cols);
#else
    // TODO move to a better way
    return sscanf(string, format, rows, cols);
#endif
}

}  // namespace Term::Private
