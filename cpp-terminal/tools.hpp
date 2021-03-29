#pragma once

#include <iostream>

namespace Term {
/*----------------------------------------------------------------------------*/

    uint8_t utf8_decode_step(uint8_t state,
                             uint8_t octet,
                             uint32_t *cpp);

/*----------------------------------------------------------------------------*/

    void codepoint_to_utf8(std::string &s, char32_t c);

/*----------------------------------------------------------------------------*/

// Converts an UTF8 string to UTF32.
    std::u32string utf8_to_utf32(const std::string &s);

// Converts an UTF32 string to UTF8.
    std::string utf32_to_utf8(const std::u32string &s);

// coverts a string into an integer
    int convert_string_to_int(const char *string,
                              const char *format,
                              int *rows,
                              int *cols);

}  // namespace Term