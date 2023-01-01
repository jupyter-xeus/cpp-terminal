#pragma once

#if __cplusplus >= 201703L

  #if defined(__GNUC__) && (__GNUC__ > 7)
    #define CPP_TERMINAL_NODISCARD [[nodiscard]]
  #elif defined(__clang_major__) && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ > 8))
    #define CPP_TERMINAL_NODISCARD [[nodiscard]]
  #else
    #define CPP_TERMINAL_NODISCARD
  #endif

  #if defined(__GNUC__) && (__GNUC__ > 5)
    #define CPP_TERMINAL_FALLTHROUGH [[fallthrough]]
  #elif defined(__clang_major__) && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ > 5))
    #define CPP_TERMINAL_FALLTHROUGH [[fallthrough]]
  #else
    #define CPP_TERMINAL_FALLTHROUGH
  #endif

  #if defined(__GNUC__) && (__GNUC__ > 5)
    #define CPP_TERMINAL_MAYBE_UNUSED [[maybe_unused]]
  #elif defined(__clang_major__) && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ > 5))
    #define CPP_TERMINAL_MAYBE_UNUSED [[maybe_unused]]
  #else
    #define CPP_TERMINAL_MAYBE_UNUSED
  #endif

#else
  #define CPP_TERMINAL_NODISCARD
  #define CPP_TERMINAL_FALLTHROUGH
  #define CPP_TERMINAL_MAYBE_UNUSED
#endif
