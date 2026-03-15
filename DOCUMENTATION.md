# cpp-terminal Documentation

**cpp-terminal** is a small, dependency-free C++ library for writing cross-platform
terminal applications. It supports Windows 10+, Linux, and macOS, and provides a
unified API across all platforms for colors, keyboard input, mouse input, terminal
resize handling, cursor control, and more.

---

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Getting Started](#getting-started)
- [Terminal Setup](#terminal-setup)
  - [Term::Terminal](#termterminal)
  - [Term::Option](#termoption)
- [Events and Input](#events-and-input)
  - [Term::read_event()](#termread_event)
  - [Term::Event](#termevent)
- [Keys](#keys)
  - [Term::Key](#termkey)
  - [Term::MetaKey](#termmetakey)
- [Output and I/O Streams](#output-and-io-streams)
  - [Term::cout / Term::cin / Term::cerr](#termcout--termcin--termcerr)
  - [Term::Buffer](#termbuffer)
- [Colors](#colors)
  - [Term::Color](#termcolor)
  - [color_fg() and color_bg()](#color_fg-and-color_bg)
- [Styles](#styles)
  - [Term::Style](#termstyle)
- [Cursor](#cursor)
  - [Term::Cursor](#termcursor)
  - [Cursor Movement Functions](#cursor-movement-functions)
- [Screen](#screen)
  - [Term::Screen](#termscreen)
  - [Screen Functions](#screen-functions)
- [Window](#window)
  - [Term::Window](#termwindow)
- [Mouse](#mouse)
  - [Term::Mouse and Term::Button](#termmouse-and-termbutton)
- [Focus](#focus)
  - [Term::Focus](#termfocus)
- [Terminal Information](#terminal-information)
  - [Term::Terminfo](#termterminfo)
  - [Term::tty](#termtty)
- [Geometry Primitives](#geometry-primitives)
  - [Term::Size, Term::Rows, Term::Columns](#termsize-termrows-termcolumns)
  - [Term::Position, Term::Row, Term::Column](#termposition-termrow-termcolumn)
- [Prompts](#prompts)
  - [prompt() and prompt_simple()](#prompt-and-prompt_simple)
  - [prompt_multiline()](#prompt_multiline)
- [Arguments](#arguments)
  - [Term::Arguments / Term::argv / Term::argc](#termarguments--termargv--termargc)
- [Version](#version)
- [CMake Integration](#cmake-integration)
- [Complete Example](#complete-example)
- [Platform Notes](#platform-notes)
- [Common Pitfalls](#common-pitfalls)

---

## Architecture Overview

cpp-terminal is built around three concepts:

**1. A terminal manager (`Term::terminal`)** — A global RAII object that configures
the terminal on construction and restores it to its original state on destruction.
You configure it with `setOptions()` at the start of your program. You never need
to manually clean up.

**2. An event loop (`Term::read_event()`)** — A blocking call that waits for any
terminal event (keypress, resize, mouse, paste, focus change) and returns it as a
`Term::Event`. Your application logic lives inside this loop.

**3. Output streams (`Term::cout`, `Term::cerr`, etc.)** — Drop-in replacements for
`std::cout` that work correctly across platforms in both cooked and raw mode.

The typical program structure is:

```
configure terminal → loop { read_event → handle event → write output } → exit
```

On exit (normal or via most signals), the RAII destructors restore the terminal
automatically. The library also installs signal handlers for `SIGSEGV`, `SIGABRT`,
and similar crash signals to attempt restoration.

---

## Getting Started

### Including the Library

Each header is focused on a specific feature. Include only what you need:

| Header | Provides |
|---|---|
| `cpp-terminal/terminal.hpp` | `Term::terminal`, `Term::terminal_title()` |
| `cpp-terminal/options.hpp` | `Term::Option` enum |
| `cpp-terminal/input.hpp` | `Term::read_event()` |
| `cpp-terminal/event.hpp` | `Term::Event` class |
| `cpp-terminal/key.hpp` | `Term::Key`, `Term::MetaKey` |
| `cpp-terminal/iostream.hpp` | `Term::cout`, `Term::cin`, `Term::cerr`, `Term::clog` |
| `cpp-terminal/color.hpp` | `Term::Color`, `color_fg()`, `color_bg()` |
| `cpp-terminal/style.hpp` | `Term::Style` enum, `style()` function |
| `cpp-terminal/cursor.hpp` | `Term::Cursor`, cursor movement functions |
| `cpp-terminal/screen.hpp` | `Term::Screen`, `screen_size()`, screen save/restore |
| `cpp-terminal/window.hpp` | `Term::Window` — 2D character buffer for TUI rendering |
| `cpp-terminal/mouse.hpp` | `Term::Mouse`, `Term::Button` |
| `cpp-terminal/focus.hpp` | `Term::Focus` |
| `cpp-terminal/tty.hpp` | `is_stdin_a_tty()`, `is_stdout_a_tty()`, `is_stderr_a_tty()` |
| `cpp-terminal/terminfo.hpp` | `Term::Terminfo` — terminal capabilities |
| `cpp-terminal/size.hpp` | `Term::Size`, `Term::Rows`, `Term::Columns` |
| `cpp-terminal/position.hpp` | `Term::Position`, `Term::Row`, `Term::Column` |
| `cpp-terminal/prompt.hpp` | `prompt()`, `prompt_simple()`, `prompt_multiline()` |
| `cpp-terminal/args.hpp` | `Term::argv`, `Term::argc` |
| `cpp-terminal/version.hpp` | `Term::Version::string()` etc. |

### Minimal Program

```cpp
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/event.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/iostream.hpp"

int main()
{
    Term::terminal.setOptions(
        Term::Option::Raw,
        Term::Option::NoClearScreen,
        Term::Option::NoSignalKeys,
        Term::Option::Cursor
    );

    while(true)
    {
        Term::Event event = Term::read_event();
        if(event.type() == Term::Event::Type::Key)
        {
            Term::Key key(event);
            if(key == Term::Key::Ctrl_C || key == 'q') break;
            if(key.isprint()) Term::cout << key.str() << std::flush;
        }
    }
    return 0;
}
```

---

## Terminal Setup

### Term::Terminal

**Header:** `cpp-terminal/terminal.hpp`

`Term::terminal` is a global `Term::Terminal` object that manages the terminal's
state for the lifetime of your program. It is initialized automatically when the
header is included (via a static `TerminalInitializer`). You configure it by
calling `setOptions()`.

```cpp
// Configure the terminal
Term::terminal.setOptions(
    Term::Option::Raw,
    Term::Option::NoClearScreen,
    Term::Option::NoSignalKeys,
    Term::Option::Cursor
);

// Query current options
Term::Options opts = Term::terminal.getOptions();

// Get an escape sequence to clear the screen and scroll-back buffer
std::string clear_seq = Term::terminal.clear();

// Set the terminal window title (supported by most terminal emulators)
Term::cout << Term::terminal_title("My App");
```

The terminal is restored to its original state automatically when `main()` returns.
You do not need to call any cleanup function.

`setOptions()` is variadic — pass any number of `Term::Option` values. Conflicting
options (e.g. `Raw` and `Cooked`) cancel each other out; the last one in the list
takes precedence.

---

### Term::Option

**Header:** `cpp-terminal/options.hpp`

`Term::Option` is an `enum class` that controls terminal behavior. Options come in
opposite pairs:

| Option | Opposite | Effect |
|---|---|---|
| `Raw` | `Cooked` | Raw mode: every keypress goes straight to your app with no kernel buffering, no echo, no line editing |
| `Cooked` | `Raw` | Cooked (canonical) mode: the kernel buffers a full line and sends it on Enter — the default terminal behavior |
| `ClearScreen` | `NoClearScreen` | Clear the screen on startup and restore it on exit |
| `NoClearScreen` | `ClearScreen` | Leave the screen contents alone (default) |
| `SignalKeys` | `NoSignalKeys` | Ctrl+C, Ctrl+Z etc. trigger OS signals as normal |
| `NoSignalKeys` | `SignalKeys` | Ctrl+C, Ctrl+Z etc. arrive as regular key events in your event loop instead of triggering signals |
| `Cursor` | `NoCursor` | Show the cursor (default) |
| `NoCursor` | `Cursor` | Hide the cursor; restored on exit |

**When to use `NoSignalKeys`:** If you are building an interactive application (an
editor, a game, a TUI) you almost certainly want `NoSignalKeys` so that Ctrl+C
comes to your app as `Term::Key::Ctrl_C` rather than killing the process. You then
handle it yourself:

```cpp
if(key == Term::Key::Ctrl_C) { /* clean up and exit */ }
```

---

## Events and Input

### Term::read_event()

**Header:** `cpp-terminal/input.hpp`

```cpp
Term::Event event = Term::read_event();
```

`read_event()` **blocks** until a terminal event occurs, then returns it. There is
no timeout. If you need concurrent behavior (e.g. animation alongside input), you
would need to run the event loop on a separate thread.

Events include: keypresses, terminal resize, cursor position reports, mouse events,
focus in/out, and pasted text.

---

### Term::Event

**Header:** `cpp-terminal/event.hpp`

`Term::Event` is a tagged union that can hold one of several event types.

#### Event Types

```cpp
enum class Type
{
    Empty,      // no event (should not occur in normal use)
    Key,        // a key was pressed
    Screen,     // the terminal was resized
    Cursor,     // a cursor position report was received
    Focus,      // the terminal gained or lost focus
    Mouse,      // a mouse event occurred
    CopyPaste   // text was pasted into the terminal
};
```

#### Checking the Type

```cpp
Term::Event event = Term::read_event();

switch(event.type())
{
    case Term::Event::Type::Key:
    {
        Term::Key key(event);  // extract the key
        break;
    }
    case Term::Event::Type::Screen:
    {
        Term::Screen screen(event);  // extract new dimensions
        break;
    }
    case Term::Event::Type::Mouse:
    {
        Term::Mouse mouse(event);
        break;
    }
    case Term::Event::Type::Focus:
    {
        Term::Focus focus(event);
        break;
    }
    case Term::Event::Type::CopyPaste:
    {
        std::string pasted = static_cast<std::string>(event);
        break;
    }
    default: break;
}
```

#### Extracting the Payload

There are two ways to get the data out of an event:

**Cast construction** (preferred for simple cases):
```cpp
Term::Key    key(event);     // only valid if event.type() == Type::Key
Term::Screen screen(event);  // only valid if event.type() == Type::Screen
Term::Mouse  mouse(event);   // only valid if event.type() == Type::Mouse
Term::Focus  focus(event);   // only valid if event.type() == Type::Focus
std::string  text = static_cast<std::string>(event); // for CopyPaste
```

**Pointer getters** (safe — return nullptr if wrong type):
```cpp
const Term::Key*    key    = event.get_if_key();
const Term::Screen* screen = event.get_if_screen();
const Term::Cursor* cursor = event.get_if_cursor();
const Term::Focus*  focus  = event.get_if_focus();
const Term::Mouse*  mouse  = event.get_if_mouse();
const std::string*  paste  = event.get_if_copy_paste();

if(key != nullptr) { /* use *key */ }
```

#### Checking if Empty

```cpp
if(event.empty()) { /* nothing happened */ }
```

---

## Keys

### Term::Key

**Header:** `cpp-terminal/key.hpp`

`Term::Key` wraps a 32-bit integer. For printable characters, the value is the
Unicode codepoint. For special keys (arrows, function keys, etc.), the value is
above the Unicode range (`0x10FFFF + N`).

#### Named Key Constants

All constants live in the `Term::Key` enum:

**Control characters (ASCII 0–31, 127):**
```cpp
Term::Key::Null        // 0   (Ctrl+@)
Term::Key::Ctrl_A      // 1
Term::Key::Ctrl_B      // 2
Term::Key::Ctrl_C      // 3   — if NoSignalKeys is set
Term::Key::Ctrl_D      // 4   — EOF
Term::Key::Ctrl_E      // 5
Term::Key::Ctrl_F      // 6
Term::Key::Backspace   // 8   (same as Ctrl_H)
Term::Key::Tab         // 9   (same as Ctrl_I)
Term::Key::Enter       // 13  (same as Ctrl_M)
Term::Key::Esc         // 27  (same as Ctrl_OpenBracket)
Term::Key::Del         // 127 (same as CTRL_QuestionMark)
Term::Key::Ctrl_U      // 21  — kill line
Term::Key::Ctrl_W      // 23  — kill word
Term::Key::Ctrl_Z      // 26  — suspend (if NoSignalKeys)
// ... Ctrl_A through Ctrl_Z all defined
```

**Printable ASCII** — all uppercase and lowercase letters, digits, punctuation:
```cpp
Term::Key::Space       // 32
Term::Key::a           // 97  ... Term::Key::z
Term::Key::A           // 65  ... Term::Key::Z
Term::Key::Zero        // 48  ... Term::Key::Nine
Term::Key::Period      // 46
Term::Key::Slash       // 47
// ... full set of punctuation defined
```

**Special / extended keys:**
```cpp
Term::Key::ArrowLeft
Term::Key::ArrowRight
Term::Key::ArrowUp
Term::Key::ArrowDown
Term::Key::Home
Term::Key::End
Term::Key::Insert
Term::Key::PageUp
Term::Key::PageDown
Term::Key::F1  // through Term::Key::F24
Term::Key::PrintScreen
Term::Key::Menu
Term::Key::NoKey   // sentinel value meaning "no key"
```

#### Comparing Keys

`Term::Key` has comparison operators for `Key`, `char`, `char32_t`, `int32_t`,
and `size_t`. You can compare directly to character literals:

```cpp
Term::Key key(event);

if(key == 'q')                   { /* lowercase q */ }
if(key == 'Q')                   { /* uppercase Q */ }
if(key == Term::Key::Enter)      { /* enter key */ }
if(key == Term::Key::ArrowUp)    { /* up arrow */ }
if(key == Term::Key::Ctrl_C)     { /* Ctrl+C */ }
if(key == Term::Key::F1)         { /* F1 */ }
```

#### Key Classification Methods

```cpp
key.isprint()    // true for printable characters (letters, digits, punctuation, space)
key.isalpha()    // true for letters (a-z, A-Z)
key.isdigit()    // true for 0-9
key.isxdigit()   // true for 0-9, a-f, A-F
key.isalnum()    // true for letters or digits
key.isupper()    // true for A-Z
key.islower()    // true for a-z
key.ispunct()    // true for punctuation
key.isgraph()    // true for printable non-space (alnum + punct)
key.isspace()    // true for whitespace (space, tab, newlines)
key.isblank()    // true for space or tab
key.iscntrl()    // true for control characters (0-31, 127)
key.isASCII()    // true for values 0-127
key.isExtendedASCII() // true for values 0-255
key.isunicode()  // true for valid Unicode codepoints (0 to 0x10FFFF)
key.hasCtrl()    // true if Ctrl modifier is set (excluding Tab/Enter/Backspace/Esc)
key.hasCtrlAll() // true if Ctrl modifier is set (including Tab/Enter/Backspace/Esc)
key.hasAlt()     // true if Alt modifier is set
key.empty()      // true if value is NoKey
```

#### Converting a Key to a String

```cpp
key.str()   // the UTF-8 string the key represents (for printable Unicode keys)
            // for special keys above 0x10FFFF, produces garbage — always guard with isprint() or isunicode()

key.name()  // human-readable name string: "ArrowUp", "Enter", "Ctrl+C", "a", etc.
            // safe for all key values including special keys
```

Example — echoing input safely:
```cpp
if(key.isprint()) {
    Term::cout << key.str() << std::flush;
} else {
    Term::cout << "[" << key.name() << "]" << std::flush;
}
```

#### Case Conversion

```cpp
Term::Key lower = key.tolower();  // no-op if not uppercase letter
Term::Key upper = key.toupper();  // no-op if not lowercase letter
```

---

### Term::MetaKey

**Header:** `cpp-terminal/key.hpp`

`Term::MetaKey` represents modifier bits that can be combined with a key. The two
modifiers are `Alt` and `Ctrl`.

```cpp
enum class Value : std::int32_t
{
    None = 0,
    Alt  = (1 << 22),
    Ctrl = (1 << 23)
};
```

These bits are encoded into the `Term::Key` value itself using the high bits above
the Unicode range. You rarely construct modified keys directly — instead you check
for them:

```cpp
if(key.hasAlt())  { /* Alt was held */ }
if(key.hasCtrl()) { /* Ctrl was held */ }
```

To construct a key with a modifier for comparison:
```cpp
Term::Key ctrl_a = Term::MetaKey::Value::Ctrl + Term::Key::a;
Term::Key alt_x  = Term::MetaKey::Value::Alt  + Term::Key::x;

if(key == ctrl_a) { /* Ctrl+A */ }
```

---

## Output and I/O Streams

### Term::cout / Term::cin / Term::cerr

**Header:** `cpp-terminal/iostream.hpp`

cpp-terminal provides its own I/O stream objects that work correctly across platforms,
especially on Windows where `std::cout` requires extra setup for ANSI sequences.

```cpp
Term::TOstream& Term::cout;  // output stream
Term::TOstream& Term::cerr;  // error stream (unbuffered)
Term::TOstream& Term::clog;  // log stream
Term::TIstream& Term::cin;   // input stream
```

These are used exactly like their `std` counterparts:

```cpp
Term::cout << "Hello, world!" << std::endl;
Term::cout << "Score: " << score << "\r\n" << std::flush;
Term::cout << Term::color_fg(Term::Color::Name::Red) << "Error!" 
           << Term::style(Term::Style::Reset) << std::flush;
```

**Important:** In raw mode, `\n` moves the cursor down but does **not** return to
column 1. Always use `\r\n` when you want to go to the next line from the start.
`std::endl` outputs `\n` and flushes — in raw mode you usually want `"\r\n" << std::flush` instead.

**Flushing:** `Term::cout` is line-buffered by default. Call `<< std::flush` to
force output to appear immediately without a newline.

**Using `std::cout`:** It is valid but not recommended. On Windows it may not
render ANSI escape sequences correctly without the library's console setup.
Prefer `Term::cout` throughout.

---

### Term::Buffer

**Header:** `cpp-terminal/buffer.hpp`

`Term::Buffer` is the `std::streambuf` implementation underlying `TOstream` and
`TIstream`. It has three buffering modes:

```cpp
enum class Type : std::uint8_t
{
    Unbuffered,    // writes go straight to the terminal
    LineBuffered,  // flushes on newline (default for cout)
    FullBuffered   // flushes only when the buffer is full or manually flushed
};
```

You do not normally interact with `Term::Buffer` directly — it is an internal
implementation detail of the stream objects.

---

## Colors

### Term::Color

**Header:** `cpp-terminal/color.hpp`

`Term::Color` represents a terminal color. It supports four color depths:

| Type | Description | Constructor |
|---|---|---|
| `Unset` | No color specified | `Term::Color()` |
| `Bit3` | 3-bit (8 colors) | implicit from `Bit4` conversion |
| `Bit4` | 4-bit (16 colors) | `Term::Color(Term::Color::Name::Red)` |
| `Bit8` | 8-bit (256 colors) | `Term::Color(uint8_t value)` |
| `Bit24` | 24-bit RGB (true color) | `Term::Color(uint8_t r, uint8_t g, uint8_t b)` |

cpp-terminal automatically downgrades colors to what the terminal supports. If the
terminal only supports 4-bit color and you pass a 24-bit color, it will be converted
to the nearest 4-bit color.

#### Named 4-bit Colors (Term::Color::Name)

```cpp
Term::Color::Name::Black
Term::Color::Name::Red
Term::Color::Name::Green
Term::Color::Name::Yellow
Term::Color::Name::Blue
Term::Color::Name::Magenta
Term::Color::Name::Cyan
Term::Color::Name::White
Term::Color::Name::Default       // terminal's default color
Term::Color::Name::Gray          // same as BrightBlack
Term::Color::Name::BrightBlack
Term::Color::Name::BrightRed
Term::Color::Name::BrightGreen
Term::Color::Name::BrightYellow
Term::Color::Name::BrightBlue
Term::Color::Name::BrightMagenta
Term::Color::Name::BrightCyan
Term::Color::Name::BrightWhite
```

#### Color Conversion Methods

```cpp
Term::Color color(255, 128, 0);  // orange in 24-bit
color.getType()     // Term::Color::Type::Bit24
color.to4bits()     // nearest 4-bit named color
color.to8bits()     // nearest 8-bit (0-255) value
color.to24bits()    // std::array<uint8_t, 3> — {r, g, b}
color.to3bits()     // nearest 3-bit color
```

---

### color_fg() and color_bg()

**Header:** `cpp-terminal/color.hpp`

These free functions return ANSI escape sequence strings for setting foreground and
background colors. Write them to `Term::cout` to apply color.

```cpp
// Foreground color
std::string color_fg(const Term::Color::Name& name);
std::string color_fg(const std::uint8_t& value);          // 8-bit (0-255)
std::string color_fg(const std::uint8_t& r, const std::uint8_t& g, const std::uint8_t& b);  // 24-bit
std::string color_fg(const Term::Color& color);

// Background color
std::string color_bg(const Term::Color::Name& name);
std::string color_bg(const std::uint8_t& value);
std::string color_bg(const std::uint8_t& r, const std::uint8_t& g, const std::uint8_t& b);
std::string color_bg(const Term::Color& color);
```

The function queries `Term::Terminfo` to determine what the terminal supports and
automatically downgrades the color if needed. If the terminal has no color support
it returns an empty string.

**Examples:**

```cpp
// Named 4-bit colors
Term::cout << Term::color_fg(Term::Color::Name::Red) << "red text"
           << Term::color_fg(Term::Color::Name::Default);  // reset to default

// 8-bit color (256-color palette)
Term::cout << Term::color_fg(214) << "orange";  // xterm color 214

// 24-bit true color
Term::cout << Term::color_fg(255, 128, 0) << "orange";
Term::cout << Term::color_bg(0, 0, 128)   << "navy background";

// Always reset when done
Term::cout << Term::style(Term::Style::Reset);
```

---

## Styles

### Term::Style

**Header:** `cpp-terminal/style.hpp`

`Term::Style` is an enum class for text styling. Use `Term::style()` to convert it
to an escape sequence string, then write it to `Term::cout`.

```cpp
std::string style(const Term::Style& s);
```

`Term::Style` also has a stream insertion operator, so you can write it directly:
```cpp
Term::cout << Term::Style::Bold << "bold text" << Term::Style::Reset;
```

#### Style Values

| Style | Effect | Notes |
|---|---|---|
| `Reset` | Reset all styles and colors | Use this to return to normal |
| `Bold` | Bold / thick font | |
| `Dim` | Lighter / thinner font | |
| `Italic` | Italic font | |
| `Underline` | Underline | |
| `Blink` | Slow blink (<150/min) | |
| `BlinkRapid` | Fast blink (150+/min) | Not widely supported |
| `Reversed` | Swap foreground and background | |
| `Conceal` | Hidden text | Not widely supported |
| `Crossed` | Strikethrough | Mostly supported |
| `Font0`–`Font10` | Alternative fonts | `Font10` = Fraktur/Gothic |
| `ResetBold` | Reset bold | |
| `ResetDim` | Reset dim | |
| `ResetItalic` | Reset italic | |
| `ResetUnderline` | Reset underline | |
| `ResetBlink` | Reset blink | |
| `ResetReversed` | Reset reversed | |
| `DefaultForegroundColor` | Reset FG to terminal default | |
| `DefaultBackgroundColor` | Reset BG to terminal default | |
| `Overline` | Line above text | Barely supported |
| `Superscript` | Superscript | mintty only |
| `Subscript` | Subscript | mintty only |

**Example:**
```cpp
Term::cout << Term::style(Term::Style::Bold)
           << Term::color_fg(Term::Color::Name::Cyan)
           << "Bold cyan text"
           << Term::style(Term::Style::Reset)
           << std::flush;
```

---

## Cursor

### Term::Cursor

**Header:** `cpp-terminal/cursor.hpp`

`Term::Cursor` represents a cursor position (row and column, 1-indexed).

```cpp
Term::Cursor cursor = Term::cursor_position();  // query current position
std::size_t row = cursor.row();       // 1-indexed
std::size_t col = cursor.column();    // 1-indexed
bool is_empty   = cursor.empty();     // true if row==0 and col==0

cursor == other_cursor;  // equality comparison
cursor != other_cursor;
```

`Term::cursor_position()` sends a cursor position request to the terminal and waits
for the response. It may block briefly. To handle cursor position responses in the
event loop instead, write `Term::cursor_position_report()` to `Term::cout` and
then catch `Term::Event::Type::Cursor` events.

---

### Cursor Movement Functions

**Header:** `cpp-terminal/cursor.hpp`

These functions return ANSI escape sequence strings. Write them to `Term::cout`
to move the cursor. All positions are 1-indexed (row 1, column 1 is the top-left).

```cpp
// Move to absolute position
std::string cursor_move(std::size_t row, std::size_t column);

// Move relative to current position
std::string cursor_up(std::size_t rows);
std::string cursor_down(std::size_t rows);
std::string cursor_left(std::size_t columns);
std::string cursor_right(std::size_t columns);

// Show/hide cursor
std::string cursor_on();   // show cursor
std::string cursor_off();  // hide cursor

// Request a cursor position report (triggers a Cursor event in the event loop)
std::string cursor_position_report();

// Clear from cursor to end of line
std::string clear_eol();
```

**Examples:**

```cpp
// Move cursor to row 5, column 10
Term::cout << Term::cursor_move(5, 10) << std::flush;

// Move 3 rows up, then 2 columns right
Term::cout << Term::cursor_up(3) << Term::cursor_right(2) << std::flush;

// Hide cursor during rendering, show after
Term::cout << Term::cursor_off();
// ... render frame ...
Term::cout << Term::cursor_on() << std::flush;

// Clear to end of current line
Term::cout << Term::clear_eol() << std::flush;
```

---

## Screen

### Term::Screen

**Header:** `cpp-terminal/screen.hpp`

`Term::Screen` represents terminal dimensions. It wraps a `Term::Size`.

```cpp
Term::Screen size = Term::screen_size();
std::size_t rows = size.rows();      // number of rows (height)
std::size_t cols = size.columns();   // number of columns (width)
bool empty       = size.empty();     // true if rows==0 and cols==0

size == other_size;
size != other_size;
```

When the terminal is resized, a `Term::Event::Type::Screen` event is delivered to
your event loop:

```cpp
case Term::Event::Type::Screen:
{
    Term::Screen new_size(event);
    // redraw using new_size.rows() and new_size.columns()
    break;
}
```

---

### Screen Functions

**Header:** `cpp-terminal/screen.hpp`

```cpp
// Clear the entire screen
std::string clear_screen();

// Save and restore the full screen state (alternate screen buffer)
std::string screen_save();   // switches to alternate screen
std::string screen_load();   // restores primary screen

// Get current terminal size
Term::Screen screen_size();
```

**Screen save/restore** is the standard way to implement full-screen applications
that restore the user's terminal content on exit:

```cpp
Term::cout << Term::screen_save() << std::flush;
// ... run full-screen app ...
Term::cout << Term::screen_load() << std::flush;
```

Note: `Term::Option::ClearScreen` does this automatically when passed to
`setOptions()`.

---

## Window

### Term::Window

**Header:** `cpp-terminal/window.hpp`

`Term::Window` is an off-screen 2D character buffer that you can write to and then
render to the terminal all at once. This is the recommended approach for building
TUI (text user interface) applications because it avoids flickering — you compose
the entire frame in memory and emit it in a single `render()` call.

Characters are stored as `char32_t` (Unicode codepoints). Each cell independently
tracks its character, foreground color, background color, and style.

```cpp
// Create a window of a given size
Term::Window win(Term::Size(Term::Rows(24), Term::Columns(80)));

// Create a window matching the current terminal size
Term::Screen size = Term::screen_size();
Term::Window win(size);
```

#### Setting Cell Content

All coordinates are 1-indexed (column 1, row 1 is top-left).

```cpp
// Set a single character
win.set_char(column, row, U'A');

// Set foreground color for a cell
win.set_fg(column, row, Term::Color(Term::Color::Name::Red));
win.set_fg_reset(column, row);  // reset to default

// Set background color
win.set_bg(column, row, Term::Color(0, 128, 255));  // 24-bit blue
win.set_bg_reset(column, row);

// Set style
win.set_style(column, row, Term::Style::Bold);

// Set cursor position within the window
win.set_cursor_pos(column, row);
```

#### Printing Strings

```cpp
// Print a string starting at (column, row)
// The third argument is the max width (0 = unlimited)
// The fourth argument: if true, wraps at the window edge
win.print_str(column, row, "Hello, world!", 0, false);
```

#### Filling Regions

```cpp
// Fill a rectangular region with a color or style
// Arguments: start_col, start_row, end_col, end_row, value
win.fill_fg(1, 1, 80, 24, Term::Color(Term::Color::Name::White));
win.fill_bg(1, 1, 80, 24, Term::Color(Term::Color::Name::Blue));
win.fill_style(1, 1, 80, 24, Term::Style::Bold);
```

#### Drawing Borders

```cpp
win.print_border();                         // border around the entire window
win.print_rect(col, row, width, height);    // border around a rectangle
```

#### Clearing

```cpp
win.clear();  // reset all cells to default
```

#### Checking Bounds

```cpp
if(win.insideWindow(column, row)) { /* valid cell */ }
```

#### Rendering

```cpp
// Render the window to a string starting at terminal position (row, col)
// Third argument: if true, does a full render; if false, does a diff render
std::string output = win.render(row, col, true);
Term::cout << output << std::flush;
```

The diff render (third argument = false) compares against the previous render and
only emits escape sequences for cells that changed. This is more efficient for
frequently updated displays.

---

## Mouse

### Term::Mouse and Term::Button

**Header:** `cpp-terminal/mouse.hpp`

Mouse events arrive as `Term::Event::Type::Mouse`. Extract them with:

```cpp
case Term::Event::Type::Mouse:
{
    Term::Mouse mouse(event);
    std::size_t row = mouse.row();
    std::size_t col = mouse.column();
    Term::Button btn = mouse.getButton();
    break;
}
```

#### Term::Button

```cpp
// Button types
enum class Type : std::int8_t
{
    None,
    Left,
    Wheel,   // middle button
    Right,
    Button1, Button2, Button3,
    Button4, Button5, Button6, Button7, Button8
};

// Button actions
enum class Action : std::int8_t
{
    None,
    Pressed,
    Released,
    DoubleClicked,
    RolledUp,    // scroll wheel up
    RolledDown,  // scroll wheel down
    ToRight,     // horizontal scroll right
    ToLeft       // horizontal scroll left
};
```

#### Querying Mouse Events

```cpp
Term::Mouse mouse(event);

// Check button type and action together
if(mouse.is(Term::Button::Type::Left, Term::Button::Action::Pressed)) { /* left click */ }

// Check button type only
if(mouse.is(Term::Button::Type::Wheel)) { /* scroll wheel */ }

// Get button details separately
Term::Button btn = mouse.getButton();
btn.type()    // Term::Button::Type
btn.action()  // Term::Button::Action
```

---

## Focus

### Term::Focus

**Header:** `cpp-terminal/focus.hpp`

Focus events are fired when the terminal window gains or loses focus (supported by
most modern terminal emulators).

```cpp
case Term::Event::Type::Focus:
{
    Term::Focus focus(event);
    if(focus.in())  { /* terminal gained focus */ }
    if(focus.out()) { /* terminal lost focus   */ }

    // Or check the type explicitly
    switch(focus.type())
    {
        case Term::Focus::Type::In:      break;
        case Term::Focus::Type::Out:     break;
        case Term::Focus::Type::Unknown: break;
    }
    break;
}
```

---

## Terminal Information

### Term::Terminfo

**Header:** `cpp-terminal/terminfo.hpp`

`Term::Terminfo` is queried automatically by the library to determine what the
terminal supports. You can also query it yourself.

```cpp
// Color capability
Term::Terminfo::ColorMode mode = Term::Terminfo::getColorMode();
// Values: Unset, NoColor, Bit3, Bit4, Bit8, Bit24

// Boolean capabilities
bool has_utf8     = Term::Terminfo::get(Term::Terminfo::Bool::UTF8);
bool is_legacy    = Term::Terminfo::get(Term::Terminfo::Bool::Legacy);  // Windows legacy console
bool has_ctrl_seq = Term::Terminfo::get(Term::Terminfo::Bool::ControlSequences);

// String capabilities
std::string term_env     = Term::Terminfo::get(Term::Terminfo::String::TermEnv);     // $TERM
std::string term_name    = Term::Terminfo::get(Term::Terminfo::String::TermName);    // terminal program name
std::string term_version = Term::Terminfo::get(Term::Terminfo::String::TermVersion); // terminal version
```

`color_fg()` and `color_bg()` use `getColorMode()` internally to downgrade colors
automatically. You do not normally need to check color mode yourself unless you want
to adapt your UI based on terminal capability.

---

### Term::tty

**Header:** `cpp-terminal/tty.hpp`

Simple functions to check whether standard streams are connected to a terminal.
Useful for detecting when output is piped:

```cpp
bool stdin_is_tty  = Term::is_stdin_a_tty();
bool stdout_is_tty = Term::is_stdout_a_tty();
bool stderr_is_tty = Term::is_stderr_a_tty();
```

**Example use case:** Disable color output when stdout is piped to a file:
```cpp
if(!Term::is_stdout_a_tty()) {
    // write plain text, no ANSI sequences
}
```

---

## Geometry Primitives

### Term::Size, Term::Rows, Term::Columns

**Header:** `cpp-terminal/size.hpp`

`Term::Size` holds terminal dimensions. `Term::Rows` and `Term::Columns` are
strong typedefs wrapping `uint16_t`, implicitly convertible to `std::size_t`.

```cpp
Term::Rows    rows(24);
Term::Columns cols(80);
Term::Size    size(rows, cols);    // or Size(cols, rows) — order doesn't matter

std::size_t h = size.rows();
std::size_t w = size.columns();
std::size_t a = size.area();      // rows * columns
```

---

### Term::Position, Term::Row, Term::Column

**Header:** `cpp-terminal/position.hpp`

`Term::Position` holds a cursor position. `Term::Row` and `Term::Column` are strong
typedefs, implicitly convertible to `std::size_t`. All values are 1-indexed.

```cpp
Term::Row      row(5);
Term::Column   col(10);
Term::Position pos(row, col);   // or Position(col, row) — order doesn't matter

std::size_t r = pos.row();
std::size_t c = pos.column();
```

---

## Prompts

### prompt() and prompt_simple()

**Header:** `cpp-terminal/prompt.hpp`

High-level yes/no prompts for interactive use. These handle terminal setup
internally.

#### prompt()

A yes/no prompt with configurable options and labels.

```cpp
Term::Result result = Term::prompt(
    "Save file?",      // message
    "y",               // first option (yes)
    "n",               // second option (no)
    ": ",              // prompt indicator
    false              // immediate: if true, no Enter needed; if false, Enter required
);
```

Return values:
```cpp
enum class Result {
    Yes,      // user chose the first option
    No,       // user chose the second option
    Error,    // no terminal attached
    None,     // Enter pressed without input
    Abort,    // Ctrl+C pressed
    Invalid   // input didn't match either option
};
```

#### prompt_simple()

The simplest possible yes/no prompt. Displays `"message [y/N]: "`. All errors and
invalid input default to `No`.

```cpp
Term::Result_simple result = Term::prompt_simple("Are you sure?");

switch(result) {
    case Term::Result_simple::Yes:   /* confirmed */ break;
    case Term::Result_simple::No:    /* declined  */ break;
    case Term::Result_simple::Abort: /* Ctrl+C    */ break;
}
```

---

### prompt_multiline()

**Header:** `cpp-terminal/prompt.hpp`

A multiline text input prompt with a validation callback.

```cpp
std::vector<std::string> history;
std::function<bool(std::string)> validator = [](const std::string& s) {
    return !s.empty();  // return true to accept the input
};

std::string input = Term::prompt_multiline(">>> ", history, validator);
```

The prompt supports multiple lines separated by Enter. The validator is called each
time the user presses Enter. If it returns `true`, the prompt is submitted. If it
returns `false`, a new line is started. Press `Ctrl+D` to submit unconditionally.

`history` is updated with the submitted input and can be passed in again on
subsequent calls for command history.

---

## Arguments

### Term::Arguments / Term::argv / Term::argc

**Header:** `cpp-terminal/args.hpp`

A cross-platform replacement for `argc`/`argv` that works on Windows (where
`wmain` is needed for Unicode arguments) as well as Linux and macOS.

```cpp
#include "cpp-terminal/args.hpp"

int main()
{
    // Term::argv and Term::argc are static global objects
    std::size_t count = Term::argc;              // number of arguments
    std::string first = Term::argv[0];           // program name
    std::string arg1  = Term::argv[1];           // first argument (if it exists)

    std::vector<std::string> all = Term::argv.argv();  // all arguments as vector
}
```

On Windows this correctly handles Unicode command-line arguments. On other platforms
it wraps the standard `argc`/`argv`.

---

## Version

**Header:** `cpp-terminal/version.hpp`

```cpp
std::uint16_t major   = Term::Version::major();
std::uint16_t minor   = Term::Version::minor();
std::uint16_t patch   = Term::Version::patch();
std::string   version = Term::Version::string();  // "X.Y.Z"

std::string url = Term::homepage();  // project URL
```

---

## CMake Integration

### As a Git Submodule (Recommended)

```bash
git submodule add https://github.com/jupyter-xeus/cpp-terminal third_party/cpp-terminal
```

In your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.14)
project(my_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force static linking
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Enable compile_commands.json for editor integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(third_party/cpp-terminal)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE cpp-terminal)
```

Build:
```bash
cmake -B build
cmake --build build
```

For editor support (clangd), symlink the compile commands:
```bash
ln -s build/compile_commands.json compile_commands.json
```

You only need to re-run `cmake -B build` when `CMakeLists.txt` changes. For code
changes, `cmake --build build` is sufficient.

---

## Complete Example

A full interactive TUI skeleton demonstrating terminal setup, the event loop,
key handling, resize handling, cursor movement, colors, and clean exit:

```cpp
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/options.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/event.hpp"
#include "cpp-terminal/key.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/color.hpp"
#include "cpp-terminal/style.hpp"
#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/screen.hpp"

int main()
{
    // Configure the terminal
    Term::terminal.setOptions(
        Term::Option::Raw,           // every keypress goes straight to our loop
        Term::Option::NoClearScreen, // don't wipe the screen on start
        Term::Option::NoSignalKeys,  // Ctrl+C comes to us as a Key event
        Term::Option::NoCursor       // hide cursor during rendering
    );

    Term::Screen size = Term::screen_size();

    // Draw a simple status bar at the bottom
    auto draw_status = [&](const std::string& msg) {
        Term::cout << Term::cursor_move(size.rows(), 1)
                   << Term::color_bg(Term::Color::Name::Blue)
                   << Term::color_fg(Term::Color::Name::White)
                   << Term::style(Term::Style::Bold)
                   << msg
                   << Term::style(Term::Style::Reset)
                   << std::flush;
    };

    draw_status(" Press 'q' or Ctrl+C to quit ");

    // Event loop
    while(true)
    {
        Term::Event event = Term::read_event();

        switch(event.type())
        {
            case Term::Event::Type::Key:
            {
                Term::Key key(event);

                // Exit conditions
                if(key == 'q' || key == Term::Key::Ctrl_C) goto done;

                // Print printable characters at current cursor position
                if(key.isprint())
                {
                    Term::cout << key.str() << std::flush;
                }
                else
                {
                    // Show name of special keys in status bar
                    draw_status(" " + key.name() + " ");
                }
                break;
            }

            case Term::Event::Type::Screen:
            {
                // Terminal was resized — update dimensions and redraw
                size = Term::Screen(event);
                draw_status(" Resized: " +
                    std::to_string(static_cast<std::size_t>(size.columns())) +
                    "x" +
                    std::to_string(static_cast<std::size_t>(size.rows())));
                break;
            }

            default: break;
        }
    }

done:
    // Move to a clean line before exit
    Term::cout << Term::cursor_move(size.rows(), 1)
               << Term::style(Term::Style::Reset)
               << "\r\n"
               << std::flush;

    return 0;
    // Term::terminal destructor restores the terminal automatically
}
```

---

## Platform Notes

**Windows 10+:** Full support. The library enables `ENABLE_VIRTUAL_TERMINAL_PROCESSING`
on the Windows Console API automatically, which enables ANSI escape sequence support.
Use `Term::cout` rather than `std::cout` to ensure correct behavior.

**Windows < 10:** Not supported. These versions lack the Win32 API for raw mode and
ANSI processing.

**Windows GUI applications:** You can attach or create a console from a `WinMain`
application by including `cpp-terminal/terminal.hpp` — it handles `AllocConsole` /
`AttachConsole` internally.

**Linux / macOS:** Full support via `termios`. All features work out of the box.

**Wayland:** Fully supported on any terminal emulator running on Wayland. cpp-terminal
interacts with the terminal emulator, not the display server directly.

---

## Common Pitfalls

**`\r\n` vs `\n`:** In raw mode, `\n` only moves the cursor down — it does not
return to column 1. Use `\r\n` whenever you want to go to the beginning of the next
line. `std::endl` outputs `\n` + flush; in raw mode you usually want `"\r\n" << std::flush`.

**`NoSignalKeys` requires manual Ctrl+C handling:** If you set `NoSignalKeys`, Ctrl+C
will not kill your program. You **must** check for it in your event loop:
```cpp
if(key == Term::Key::Ctrl_C) { /* exit */ }
```
If you forget this, your program will appear to hang on Ctrl+C.

**Variable declarations in `switch` cases:** Declaring a variable inside a `case`
without braces is a C++ error if another `case` follows. Always wrap `case` bodies
that declare variables in `{}`:
```cpp
case Term::Event::Type::Key:
{
    Term::Key key(event);  // requires braces
    break;
}
```

**`key.str()` on special keys:** `str()` converts the key's value as a Unicode
codepoint to UTF-8. Special keys (arrows, F-keys, etc.) have values above `0x10FFFF`
and are not valid Unicode — `str()` will produce garbage bytes. Always guard with
`key.isprint()` or `key.isunicode()` before calling `str()`.

**Flushing:** `Term::cout` is line-buffered. Output doesn't appear until a `\n` or
an explicit flush. In raw mode where you may not be writing newlines, always end
output with `<< std::flush`.

**Terminal not restored after crash:** The RAII destructor and signal handlers cover
normal exit and most signals, but `SIGKILL` cannot be caught. If your terminal is
left in raw mode, run `reset` or `stty sane` in your shell.

**`Term::cursor_position()` may block:** Querying the cursor position sends an escape
sequence to the terminal and waits for a response. This can block if the terminal
doesn't respond. For non-blocking position tracking, update your own cursor state
variable as you move the cursor rather than querying it.
