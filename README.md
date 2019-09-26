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
into your project. Consult the examples how to use it. You can just use the
`terminal_base.h`, which is a standalone header file, if you only want the low
level platform dependent functionality. Use `terminal.h`, which depends on
`terminal_base.h`, if you also want the platform independent code to more
easily print escape sequences and/or read and translate key codes.

## Documentation

We will start from the simplest concept (just printing a text on the screen)
and then we will keep adding more features such as colors, cursor movement,
keyboard input, etc., and we will be explaining how things work as we go.

### Printing

To print text into standard output, one can use `std::cout` in C++:
```c++
std::cout << "Some text" << std::endl;
```
One does not need `Terminal` for that.

### Colors

To print colors and other styles (such as bold), use the `Term::color()`
function and `Term::fg` enum for foreground, `Term::bg` enum for background and
`Term::style` enum for different styles (see the `colors.cpp` example):
```c++
#include "terminal.h"
using Term::color;
using Term::fg;
using Term::bg;
using Term::style;
int main() {
    try {
        Term::Terminal term;
        std::string text = "Some text with "
            + color(fg::red) + color(bg::green) + "red on green"
            + color(bg::reset) + color(fg::reset) + " and some "
            + color(style::bold) + "bold text" + color(style::reset) + ".";
        std::cout << text << std::endl;
    } catch(...) {
        throw;
    }
    return 0;
}
```
One must call `Term::fg::reset`, `Term::bg::reset` and `Term::style::reset` to
reset the given color or style.

One must create the `Term::Terminal` instance. In this case, the `Terminal`
does nothing on Linux and macOS, but on Windows it checks if the program is
running withing the Windows console and if so, enables ANSI escape codes in the
console, which makes the console show colors properly. One must have a
`try/catch` block in the main program to ensure the `Terminal`'s destructor
gets called (even if an unhandled exception occurs), which will put the console
into the original mode.

The program might decide to print colors not only if it is in a terminal (which
can be checked by `term.is_stdout_a_tty()`), but also when not run in a
terminal, some examples:

* Running on a CI, e.g. AppVeyor, Travis-CI and Azure Pipelines all show colors
  properly
* Using `less -r` shows colors properly (but `less` does not)
* Printing colors in program output in a Jupyter notebook (and then possibly
  converting such colors from ANSI sequences to html)

An example when the program might not print colors is when the standard output
gets redirected to a file (say, compiler error messages using `g++ a.cpp >
log`), and then the file is read directly in some editor.

The `color()` function always returns a string with the proper ANSI sequence.
The program might wrap this in a macro, that will check some program variable
if it should print colors and only call `color()` if colors should be printed.

### Cursor movement and its visibility

The next step up is to allow cursor movement and other ANSI sequences. For
example, here is how to render a simple menu (see `menu.cpp` example) and print
it on the screen:
```c++
void render(int rows, int cols, int pos)
{
    std::string scr;
    scr.reserve(16*1024);

    scr.append(cursor_off());
    scr.append(move_cursor(1, 1));

    for (int i=1; i <= rows; i++) {
        if (i == pos) {
            scr.append(color(fg::red));
            scr.append(color(bg::gray));
            scr.append(color(style::bold));
        } else {
            scr.append(color(fg::blue));
            scr.append(color(bg::green));
        }
        scr.append(std::to_string(i) + ": item");
        scr.append(color(bg::reset));
        scr.append(color(fg::reset));
        scr.append(color(style::reset));
        if (i < rows) scr.append("\n");
    }

    scr.append(move_cursor(rows / 2, cols / 2));

    scr.append(cursor_on());

    std::cout << scr << std::flush;
}
```
This will accumulate the following operations into a string:

* Turn off the cursor (so that the terminal does not show the cursor
  quickly moving around the screen)
* Move the cursor to the `(1,1)` position
* Print the menu in color and highlighting the selected item (specified by
  `pos`)
* Move the cursor to the middle of the screen
* Turn on the cursor

and print the string. The `std::flush` ensures that the whole string ends up on
the screen.

### Saving the original screen and restoring it

It is a good habit to restore the original terminal screen (and cursor
position) if we are going move the cursor around and draw (as in the previous
section). To do that, call the `save_screen()` method:
```c++
Term::Terminal term;
term.save_screen();
```
This issues the proper ANSI sequences to the terminal to save the screen. The
`Terminal`'s destructor will then automatically issue the corresponding
sequences to restore the original screen and the cursor position.

### Keyboard input

The final step is to enable keyboard input. To do that, one must set the
terminal in a so called "raw" mode:
```c++
Terminal term(true);
```
On Linux and macOS, this disables terminal input buffering, thus every key
press is immediately sent to the application (otherwise one has to press ENTER
before any input is sent). On Windows, this turns on ANSI keyboard sequences
for key presses.

The `Terminal`'s destructor then properly restores the terminal to the original
mode on all platforms.

One can then wait and read individual keys and do something based on
that, such as (see `menu.cpp`):
```c++
int key = term.read_key();
switch (key) {
    case Key::ARROW_UP: if (pos > 1) pos--; break;
    case Key::ARROW_DOWN: if (pos < rows) pos++; break;
    case 'q':
    case Key::ESC:
          on = false; break;
}
```

Now we have all the features that are needed to write any terminal application.
See `kilo.cpp` for an example of a simple full screen editor.

## Similar Projects

### Colors

Libraries to handle color output.

C++:

* [rang](https://github.com/agauniyal/rang)

### Drawing

JavaScript:

* [node-drawille](https://github.com/madbence/node-drawille)

### Prompt

Libraries to handle a prompt in terminals.

C and C++:

* [readline](https://tiswww.case.edu/php/chet/readline/rltop.html)
* [libedit](http://thrysoee.dk/editline/)
* [linenoise](https://github.com/antirez/linenoise)
* [replxx](https://github.com/AmokHuginnsson/replxx)

Python:

* [python-prompt-toolkit](https://github.com/prompt-toolkit/python-prompt-toolkit)

### General TUI libraries

C and C++:

* [curses](https://en.wikipedia.org/wiki/Curses_%28programming_library%29) and [ncurses](https://www.gnu.org/software/ncurses/ncurses.html)
* [Newt](https://en.wikipedia.org/wiki/Newt_(programming_library))
* [termbox](https://github.com/nsf/termbox)

Python:

* [urwid](http://urwid.org/)
* [python-prompt-toolkit](https://github.com/prompt-toolkit/python-prompt-toolkit)
* [npyscreen](http://www.npcole.com/npyscreen/)
* [curtsies](https://github.com/bpython/curtsies)

Go:

* [gocui](https://github.com/jroimartin/gocui)
* [clui](https://github.com/VladimirMarkelov/clui)
* [tview](https://github.com/rivo/tview)
* [termbox-go](https://github.com/nsf/termbox-go)
* [termui](https://github.com/gizak/termui)
* [tcell](https://github.com/gdamore/tcell)

Rust:

* [tui-rs](https://github.com/fdehau/tui-rs)

JavaScript:

* [blessed](https://github.com/chjj/blessed) and [blessed-contrib](https://github.com/yaronn/blessed-contrib)
