add_executable(prompt prompt.cpp)
target_link_libraries(prompt terminal)

add_executable(kilo kilo.cpp)
target_link_libraries(kilo terminal)

add_executable(menu menu.cpp)
target_link_libraries(menu terminal)

add_executable(menu_window menu_window.cpp)
target_link_libraries(menu_window terminal)

add_executable(keys keys.cpp)
target_link_libraries(keys terminal)

add_executable(colors colors.cpp)
target_link_libraries(colors terminal)
