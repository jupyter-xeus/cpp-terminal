#pragma once

#if __cplusplus >= 201703L
    #if defined(__GNUC__) && (__GNUC__ > 7 )
        #define CPP_TERMINAL_NODISCARD [[nodiscard]]
    #elif defined(__clang_major__) && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ > 8))
        #define CPP_TERMINAL_NODISCARD [[nodiscard]]
    #else
        #define CPP_TERMINAL_NODISCARD
    #endif
#else
    #define CPP_TERMINAL_NODISCARD
#endif
