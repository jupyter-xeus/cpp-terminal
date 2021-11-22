#ifndef TERMINAL_BASE_H
#define TERMINAL_BASE_H

/*
 * This file contains all the platform specific code regarding terminal input
 * and output. The rest of the code does not have any platform specific
 * details. This file is designed in a way to contain the least number of
 * building blocks, that the rest of the code can use to build all the
 * features.
 */

#include <stdexcept>

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#undef B0
#undef B50
#undef B75
#undef B110
#undef B134
#undef B150
#undef B200
#undef B300
#undef B600
#undef B1200
#undef B1800
#undef B2400
#undef B4800
#undef B9600
#undef B19200
#undef B28800
#undef B38400
#undef B57600
#undef B115200
#include <unistd.h>
#include <cerrno>
#endif

namespace Term {



}  // namespace Term

#endif  // TERMINAL_BASE_H
