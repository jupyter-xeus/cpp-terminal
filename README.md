# Terminal

`Terminal` is small header only library for writing terminal applications. It
works on Linux, macOS and Windows (in the native `cmd.exe` console). It
supports colors, keyboard input and has all the basic features to write any
terminal application.

It has a small core ([terminal_base.h](terminal_base.h)) that has a few
platform specific building blocks, and a platform independent library written
on top using the ANSI escape sequences ([terminal.h](terminal.h)).

This design has the advantage of having only a few lines to maintain on each
platform, and the rest is platform independent. We intentionally limit
ourselves to a subset of features that all work on all platforms natively. That
way, any application written using `Terminal` will work everywhere out of the
box, without emulation. At the same time, because the code of `Terminal` is
short, one can easily debug it if something does not work, and have a full
understanding how things work underneath.

## Examples

Several examples are provided to show how to use `Terminal`. Every example
works natively on all platforms:

* [kilo.cpp](examples/kilo.cpp): the [kilo](https://github.com/snaptoken/kilo-src) text editor
  ported to C++ and `Terminal` instead of using Linux specific API.
* [menu.cpp](examples/menu.cpp): Shows a menu on the screen
* [keys.cpp](examples/keys.cpp): Listens for keys, showing their numbers
* [colors.cpp](examples/colors.cpp): Shows how to print text in color to standard output

## How to use

The easiest is to just copy the two files `terminal.h` and `terminal_base.h`
into your project. Consult the examples how to use it.
