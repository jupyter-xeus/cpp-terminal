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
    "$<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,4.9>:-fdiagnostics-color=auto>"
    "-pedantic"
    "-Wall"
    "-Wcast-align"
    "-Wconversion"
    "-Wextra"
    "-Wno-ignored-qualifiers"
    "-Wno-missing-field-initializers"
    "-Wno-unknown-pragmas"
    "-Woverloaded-virtual"
    "-Wpedantic"
    "-Wreorder"
    "-Wshadow"
    "-Wsign-compare"
    "-Wsign-conversion"
    "-Wstrict-aliasing"
    "-Wuninitialized"
    "-Wunreachable-code"
    "-Wunused-parameter"
    )
set(GCC_LIKE_CXX_WARNINGS
    "-Wnon-virtual-dtor"
    "-Wzero-as-null-pointer-constant"
    )
#GCC
set(GCC_WARNINGS
    "-fdiagnostics-color=auto"
    "-Wno-implicit-fallthrough"
    "-Wno-maybe-uninitialized"
    "-Wno-strict-overflow"
    "-Wpointer-arith"
    "-Wredundant-decls"
    "-Wundef"
    "-Wwrite-strings"
    )
set(GCC_CXX_WARNINGS
    "-Wdelete-non-virtual-dtor"
    "-Wnoexcept"
    )
#Clang like
set(CLANG_LIKE_WARNINGS
    "--extra-warnings"
    "-fcolor-diagnostics"
    "-pedantic"
    "-Wbool-conversion"
    "-Wconditional-uninitialized"
    "-Wconstant-conversion"
    "-Wextra-semi"
    "-Wint-conversion"
    "-Wnullable-to-nonnull-conversion"
    "-Wshadow-all"
    "-Wshift-sign-overflow"
    "-Wshorten-64-to-32"
    "-Wunused-variable"
    )
#Clang
set(CLANG_CXX_WARNINGS
    "-Winconsistent-missing-destructor-override"
    "-Woverloaded-virtual"
    "-Wunused-private-field"
    )
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
    "-Wno-debug-disables-optimization"
    )
set(INTEL_WARNINGS_WINDOWS
    "/W5"
    "/Wall"
    )
#MSVC
set(MSVC_WARNINGS
    "/W4"
    "/Wall"
    "/WL"
    )

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
