set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

# configure version information
configure_file(version.cpp.in version.cpp)

add_library(cpp-terminal-private STATIC "${CMAKE_CURRENT_BINARY_DIR}/version.cpp" return_code.cpp file_initializer.cpp exception.cpp unicode.cpp conversion.cpp args.cpp terminal_impl.cpp tty.cpp terminfo.cpp input.cpp screen.cpp cursor.cpp file.cpp env.cpp blocking_queue.cpp sigwinch.cpp)
target_link_libraries(cpp-terminal-private PRIVATE Warnings::Warnings PUBLIC Threads::Threads)
target_compile_options(cpp-terminal-private PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /wd4668 /wd4514>)
target_include_directories(cpp-terminal-private PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}> $<INSTALL_INTERFACE:include>)
add_library(cpp-terminal::cpp-terminal-private ALIAS cpp-terminal-private)
