set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED MODULE)

# configure version information
configure_file(version.cpp.in version.cpp)
add_library(cpp-terminal-private INTERFACE)
target_sources(cpp-terminal-private INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/version.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/return_code.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/file_initializer.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/exception.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/unicode.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/conversion.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/args.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/terminal_impl.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tty.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/terminfo.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/input.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/screen.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cursor.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/file.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/env.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/blocking_queue.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sigwinch.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/signals.cpp>
)
target_link_libraries(cpp-terminal-private INTERFACE Warnings::Warnings Threads::Threads)
target_compile_options(cpp-terminal-private INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_include_directories(cpp-terminal-private INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
add_library(cpp-terminal::cpp-terminal-private ALIAS cpp-terminal-private)
