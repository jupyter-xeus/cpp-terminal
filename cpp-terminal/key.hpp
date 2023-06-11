#pragma once

#include <cstdint>
#include <string>

namespace Term
{

class Key
{
public:
  enum Value : std::int32_t
  {
    NO_KEY             = -1,
    // Begin ASCII (some ASCII names has been change to their CTRL + key part)
    CTRL_AROBASE       = 0,
    CTRL_A             = 1,
    CTRL_B             = 2,
    CTRL_C             = 3,
    CTRL_D             = 4,
    CTRL_E             = 5,
    CTRL_F             = 6,
    CTRL_G             = 7,
    CTRL_H             = 8,
    CTRL_I             = 9,
    CTRL_J             = 10,
    CTRL_K             = 11,
    CTRL_L             = 12,
    CTRL_M             = 13,
    CTRL_N             = 14,
    CTRL_O             = 15,
    CTRL_P             = 16,
    CTRL_Q             = 17,
    CTRL_R             = 18,
    CTRL_S             = 19,
    CTRL_T             = 20,
    CTRL_U             = 21,
    CTRL_V             = 22,
    CTRL_W             = 23,
    CTRL_X             = 24,
    CTRL_Y             = 25,
    CTRL_Z             = 26,
    CTRL_OPEN_BRACKET  = 27,
    CTRL_BACKSLASH     = 28,
    CTRL_CLOSE_BRACKET = 29,
    CTRL_CARET         = 30,
    CTRL_UNDERSCORE    = 31,
    SPACE              = 32,
    EXCLAMATION_MARK   = 33,
    QUOTE              = 34,
    HASH               = 35,
    DOLLAR             = 36,
    PERCENT            = 37,
    AMPERSAND          = 38,
    APOSTROPHE         = 39,
    OPEN_PARENTHESIS   = 40,
    CLOSE_PARENTHESIS  = 41,
    ASTERISK           = 42,
    PLUS               = 43,
    COMMA              = 44,
    HYPHEN             = 45,
    MINUS              = 45,
    PERIOD             = 46,
    SLASH              = 47,
    ZERO               = 48,
    ONE                = 49,
    TWO                = 50,
    THREE              = 51,
    FOUR               = 52,
    FIVE               = 53,
    SIX                = 54,
    SEVEN              = 55,
    EIGHT              = 56,
    NINE               = 57,
    COLON              = 58,
    SEMICOLON          = 59,
    LESS_THAN          = 60,
    OPEN_CHEVRON       = 60,
    EQUAL              = 61,
    GREATER_THAN       = 62,
    CLOSE_CHEVRON      = 62,
    QUESTION_MARK      = 63,
    AROBASE            = 64,
    A                  = 65,
    B                  = 66,
    C                  = 67,
    D                  = 68,
    E                  = 69,
    F                  = 70,
    G                  = 71,
    H                  = 72,
    I                  = 73,
    J                  = 74,
    K                  = 75,
    L                  = 76,
    M                  = 77,
    N                  = 78,
    O                  = 79,
    P                  = 80,
    Q                  = 81,
    R                  = 82,
    S                  = 83,
    T                  = 84,
    U                  = 85,
    V                  = 86,
    W                  = 87,
    X                  = 88,
    Y                  = 89,
    Z                  = 90,
    OPEN_BRACKET       = 91,
    BACKSLASH          = 92,
    CLOSE_BRACKET      = 93,
    CARET              = 94,
    UNDERSCORE         = 95,
    GRAVE_ACCENT       = 96,
    a                  = 97,
    b                  = 98,
    c                  = 99,
    d                  = 100,
    e                  = 101,
    f                  = 102,
    g                  = 103,
    h                  = 104,
    i                  = 105,
    j                  = 106,
    k                  = 107,
    l                  = 108,
    m                  = 109,
    n                  = 110,
    o                  = 111,
    p                  = 112,
    q                  = 113,
    r                  = 114,
    s                  = 115,
    t                  = 116,
    u                  = 117,
    v                  = 118,
    w                  = 119,
    x                  = 120,
    y                  = 121,
    z                  = 122,
    OPEN_BRACE         = 123,
    VERTICAL_BAR       = 124,
    CLOSE_BRACE        = 125,
    TILDE              = 126,
    CTRL_QUESTION_MARK = 127,
    // Very useful CTRL_* alternative names
    NUL                = 0,
    BACKSPACE          = 8,
    TAB                = 9,
    ENTER              = 13,
    ESC                = 27,
    DEL                = 127,
    //
    // End ASCII
    // Extended ASCII goes up to 255
    // Last Unicode codepage 0x10FFFF
    ARROW_LEFT         = 0x10FFFF + 1,
    ARROW_RIGHT        = 0x10FFFF + 2,
    ARROW_UP           = 0x10FFFF + 3,
    ARROW_DOWN         = 0x10FFFF + 4,
    NUMERIC_5          = 0x10FFFF + 5,
    HOME               = 0x10FFFF + 6,
    INSERT             = 0x10FFFF + 7,
    END                = 0x10FFFF + 8,
    PAGE_UP            = 0x10FFFF + 9,
    PAGE_DOWN          = 0x10FFFF + 10,
    F1                 = 0x10FFFF + 11,
    F2                 = 0x10FFFF + 12,
    F3                 = 0x10FFFF + 13,
    F4                 = 0x10FFFF + 14,
    F5                 = 0x10FFFF + 15,
    F6                 = 0x10FFFF + 16,
    F7                 = 0x10FFFF + 17,
    F8                 = 0x10FFFF + 18,
    F9                 = 0x10FFFF + 19,
    F10                = 0x10FFFF + 20,
    F11                = 0x10FFFF + 21,
    F12                = 0x10FFFF + 22,
    F13                = 0x10FFFF + 23,
    F14                = 0x10FFFF + 24,
    F15                = 0x10FFFF + 25,
    F16                = 0x10FFFF + 26,
    F17                = 0x10FFFF + 27,
    F18                = 0x10FFFF + 28,
    F19                = 0x10FFFF + 29,
    F20                = 0x10FFFF + 30,
    F21                = 0x10FFFF + 31,
    F22                = 0x10FFFF + 32,
    F23                = 0x10FFFF + 33,
    F24                = 0x10FFFF + 34,
    PRINT_SCREEN       = 0x10FFFF + 35,
    // Now use << to for detecting special key + key press
    ALT                = (1 << 22)
  };
  Key() = default;
  explicit Key(const Term::Key::Value& value);
  // clang-format off
  operator Term::Key::Value();
  // clang-format on

  bool iscntrl();
  bool isblank();
  bool isspace();
  bool isupper();
  bool islower();
  bool isalpha();
  bool isdigit();
  bool isxdigit();
  bool isalnum();
  bool ispunct();
  bool isgraph();
  bool isprint();

  char tolower();
  char toupper();

  // Detect if Key is convertible to ANSII
  bool isASCII();
  // Detect if Key is convertible to Extended ANSII
  bool isExtendedASCII();
  // Detect if Key is CTRL+*
  bool isCTRL();
  // Detect if Key is ALT+*
  bool isALT();
  bool empty();

  std::string str();
  char32_t    codepoint();

private:
  Value m_value{NO_KEY};
};

}  // namespace Term
