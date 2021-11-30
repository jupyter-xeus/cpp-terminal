add_executable(prompt_multiline prompt_multiline.cpp)
target_link_libraries(prompt_multiline cpp-terminal)

add_executable(prompt_blocking prompt_blocking.cpp)
target_link_libraries(prompt_blocking cpp-terminal)

add_executable(prompt_non_blocking prompt_non_blocking.cpp)
target_link_libraries(prompt_non_blocking cpp-terminal)

add_executable(prompt_simple prompt_simple.cpp)
target_link_libraries(prompt_simple cpp-terminal)

add_executable(kilo kilo.cpp)
target_link_libraries(kilo cpp-terminal)

add_executable(menu menu.cpp)
target_link_libraries(menu cpp-terminal)

add_executable(menu_window menu_window.cpp)
target_link_libraries(menu_window cpp-terminal)

add_executable(menu_window_24bit menu_window_24bit.cpp)
target_link_libraries(menu_window_24bit cpp-terminal)

add_executable(keys keys.cpp)
target_link_libraries(keys cpp-terminal)

add_executable(colors colors.cpp)
target_link_libraries(colors cpp-terminal)

# enable warnings and set compile features
foreach(target kilo menu menu_window keys colors)
        # Force Microsoft Visual Studio to decode sources files in UTF-8
        if (MSVC)
                target_compile_options(${target} PUBLIC "/utf-8")
        endif()
        # Add as many warning as possible:
        if (WIN32)
                if (MSVC)
                        target_compile_options(${target} PRIVATE "/W4")
                        target_compile_options(${target} PRIVATE "/WX")
                        target_compile_options(${target} PRIVATE "/wd4244")
                        target_compile_options(${target} PRIVATE "/wd4267")
                        target_compile_options(${target} PRIVATE "/D_CRT_SECURE_NO_WARNINGS")
                endif()
                # Force Win32 to UNICODE
                target_compile_definitions(${target} PRIVATE UNICODE _UNICODE)
        else()
                target_compile_options(${target} PRIVATE "-Wall")
                target_compile_options(${target} PRIVATE "-Wextra")
                target_compile_options(${target} PRIVATE "-pedantic")
                target_compile_options(${target} PRIVATE "-Werror")
                target_compile_options(${target} PRIVATE "-Wno-sign-compare")
        endif()
endforeach()