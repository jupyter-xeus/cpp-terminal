#[=======================================================================[.rst:

Warnings
-------------------------

Provides a helper target for configuring some default compiler warnings.

The warning flags are designed to be as strict, verbose, and complete as possible, but "warnings as errors" is not configured by this module.

Targets
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
``Warnings::Warnings``

Interface target with default warning flags configured.

#]=======================================================================]

add_library(${PROJECT_NAME}Warnings INTERFACE)

#GCC like
set(GCC_LIKE_WARNINGS
    "-Wall"
    "-Wextra")
set(GCC_LIKE_CXX_WARNINGS)
#GCC
set(GCC_WARNINGS
        "$<$<VERSION_GREATER_EQUAL:$<C_COMPILER_VERSION>,4.8.0>:-Wpedantic>"
        "$<$<VERSION_GREATER_EQUAL:$<C_COMPILER_VERSION>,4.9.0>:-fdiagnostics-color=auto>"
    "-Wpointer-arith"
    "-Wredundant-decls"
    "-Wundef"
    "-Wwrite-strings")
set(GCC_CXX_WARNINGS
    "-Wdelete-non-virtual-dtor"
        "$<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,4.8.0>:-Wpedantic>"
        "$<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,4.9.0>:-fdiagnostics-color=auto>"
    "-Wnoexcept")
#Clang like
set(CLANG_LIKE_WARNINGS
    "-Wno-c++98-compat")
#Clang
set(CLANG_CXX_WARNINGS)
#Intel
set(INTEL_WARNINGS
    "-w3"
    "-Wall"
    "-Wextra-tokens"
    "-Wformat"
    "-Wmain"
    "-Wpointer-arith"
    "-Wreorder"
    "-Wreturn-type"
    "-Wshadow"
    "-Wsign-compare"
    "-Wstrict-aliasing"
    "-Wuninitialized"
    "-Wwrite-strings"
    "-Wno-debug-disables-optimization")
set(INTEL_WARNINGS_WINDOWS
    "/W5"
    "/Wall")
#MSVC
set(MSVC_WARNINGS
        "/W4"
        "/Wall"
        "/WL"
        "/wd5045"
        "/wd4820"
        "/wd4514")

target_compile_options(
  ${PROJECT_NAME}Warnings
  INTERFACE
  "$<$<C_COMPILER_ID:MSVC>:${MSVC_WARNINGS}>"
  "$<$<C_COMPILER_ID:GNU,Clang,AppleClang>:${GCC_LIKE_WARNINGS}>"
  "$<$<C_COMPILER_ID:GNU>:${GCC_WARNINGS}>"
  "$<$<C_COMPILER_ID:Clang,AppleClang>:${CLANG_LIKE_WARNINGS}>"
  "$<$<C_COMPILER_ID:Intel,IntelLLVM>:$<IF:$<PLATFORM_ID:Windows>,${INTEL_WARNINGS_WINDOWS},${INTEL_WARNINGS}>>"
  "$<$<CXX_COMPILER_ID:MSVC>:${MSVC_WARNINGS}>"
  "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:${GCC_LIKE_WARNINGS}>"
  "$<$<CXX_COMPILER_ID:GNU>:${GCC_WARNINGS}>"
  "$<$<CXX_COMPILER_ID:Clang,AppleClang>:${CLANG_LIKE_WARNINGS}>"
  "$<$<OR:$<COMPILE_LANG_AND_ID:CXX,Clang,AppleClang,GNU>,$<COMPILE_LANG_AND_ID:OBJCXX,Clang,AppleClang,GNU>>:${GCC_LIKE_CXX_WARNINGS}>"
  "$<$<OR:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<COMPILE_LANG_AND_ID:OBJCXX,GNU>>:${GCC_CXX_WARNINGS}>"
  "$<$<OR:$<COMPILE_LANG_AND_ID:CXX,Clang,AppleClang>,$<COMPILE_LANG_AND_ID:OBJCXX,Clang,AppleClang>>:${CLANG_CXX_WARNINGS}>"
  "$<$<CXX_COMPILER_ID:Intel,IntelLLVM>:$<IF:$<PLATFORM_ID:Windows>,${INTEL_WARNINGS_WINDOWS},${INTEL_WARNINGS}>>"
)

add_library(Warnings::Warnings ALIAS ${PROJECT_NAME}Warnings)

unset(GCC_LIKE_WARNINGS)
unset(GCC_LIKE_CXX_WARNINGS)
unset(GCC_WARNINGS)
unset(GCC_CXX_WARNINGS)
unset(CLANG_LIKE_WARNINGS)
unset(CLANG_CXX_WARNINGS)
unset(INTEL_WARNINGS)
unset(INTEL_WARNINGS_WINDOWS)
unset(MSVC_WARNINGS)
