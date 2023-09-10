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
    NoKey = -1,
    // Now use << to for detecting special key + key press

    // Begin ASCII (some ASCII names has been change to their Ctrl_+key part) the value is different to be able to do
    Ctrl_Arobase      = 0,
    Ctrl_A            = 1,
    Ctrl_B            = 2,
    Ctrl_C            = 3,
    Ctrl_D            = 4,
    Ctrl_E            = 5,
    Ctrl_F            = 6,
    Ctrl_G            = 7,
    Ctrl_H            = 8,
    Ctrl_I            = 9,
    Ctrl_J            = 10,
    Ctrl_K            = 11,
    Ctrl_L            = 12,
    Ctrl_M            = 13,
    Ctrl_N            = 14,
    Ctrl_O            = 15,
    Ctrl_P            = 16,
    Ctrl_Q            = 17,
    Ctrl_R            = 18,
    Ctrl_S            = 19,
    Ctrl_T            = 20,
    Ctrl_U            = 21,
    Ctrl_V            = 22,
    Ctrl_W            = 23,
    Ctrl_X            = 24,
    Ctrl_Y            = 25,
    Ctrl_Z            = 26,
    Ctrl_OpenBracket  = 27,
    Ctrl_BackSlash    = 28,
    Ctrl_CloseBracket = 29,
    Ctrl_Caret        = 30,
    Ctrl_Underscore   = 31,
    Space             = 32,
    ExclamationMark   = 33,
    Quote             = 34,
    Hah               = 35,
    Dollar            = 36,
    Percent           = 37,
    Ampersand         = 38,
    Apostrophe        = 39,
    OpenParenthesis   = 40,
    CloseParenthesis  = 41,
    Asterisk          = 42,
    Plus              = 43,
    Comma             = 44,
    Hyphen            = 45,
    Minus             = 45,
    Period            = 46,
    Slash             = 47,
    Zero              = 48,
    One               = 49,
    Two               = 50,
    Three             = 51,
    Four              = 52,
    Five              = 53,
    Six               = 54,
    Seven             = 55,
    Eight             = 56,
    Nine              = 57,
    Colon             = 58,
    Semicolon         = 59,
    LessThan          = 60,
    OpenChevron       = 60,
    Equal             = 61,
    GreaterThan       = 62,
    CloseChevron      = 62,
    QuestionMark      = 63,
    Arobase           = 64,
    A                 = 65,
    B                 = 66,
    C                 = 67,
    D                 = 68,
    E                 = 69,
    F                 = 70,
    G                 = 71,
    H                 = 72,
    I                 = 73,
    J                 = 74,
    K                 = 75,
    L                 = 76,
    M                 = 77,
    N                 = 78,
    O                 = 79,
    P                 = 80,
    Q                 = 81,
    R                 = 82,
    S                 = 83,
    T                 = 84,
    U                 = 85,
    V                 = 86,
    W                 = 87,
    X                 = 88,
    Y                 = 89,
    Z                 = 90,
    OpenBracket       = 91,
    Backslash         = 92,
    CloseBracket      = 93,
    Caret             = 94,
    Underscore        = 95,
    GraveAccent       = 96,
    a                 = 97,
    b                 = 98,
    c                 = 99,
    d                 = 100,
    e                 = 101,
    f                 = 102,
    g                 = 103,
    h                 = 104,
    i                 = 105,
    j                 = 106,
    k                 = 107,
    l                 = 108,
    m                 = 109,
    n                 = 110,
    o                 = 111,
    p                 = 112,
    q                 = 113,
    r                 = 114,
    s                 = 115,
    t                 = 116,
    u                 = 117,
    v                 = 118,
    w                 = 119,
    x                 = 120,
    y                 = 121,
    z                 = 122,
    OpenBrace         = 123,
    VerticalBar       = 124,
    Close_Brace       = 125,
    Tilde             = 126,
    CTRL_QuestionMark = 127,
    // Very useful CTRL_* alternative names
    Null              = 0,
    Backspace         = 8,
    Tab               = 9,
    Enter             = 13,
    Esc               = 27,
    Del               = 127,
    //
    // End ASCII
    // Extended ASCII goes up to 255
    // Last Unicode codepage 0x10FFFF
    ArrowLeft         = 0x10FFFF + 1,
    ArrowRight        = 0x10FFFF + 2,
    ArrowUp           = 0x10FFFF + 3,
    ArrowDown         = 0x10FFFF + 4,
    Numeric5          = 0x10FFFF + 5,
    Home              = 0x10FFFF + 6,
    Insert            = 0x10FFFF + 7,
    End               = 0x10FFFF + 8,
    PageUp            = 0x10FFFF + 9,
    PageDown          = 0x10FFFF + 10,
    F1                = 0x10FFFF + 11,
    F2                = 0x10FFFF + 12,
    F3                = 0x10FFFF + 13,
    F4                = 0x10FFFF + 14,
    F5                = 0x10FFFF + 15,
    F6                = 0x10FFFF + 16,
    F7                = 0x10FFFF + 17,
    F8                = 0x10FFFF + 18,
    F9                = 0x10FFFF + 19,
    F10               = 0x10FFFF + 20,
    F11               = 0x10FFFF + 21,
    F12               = 0x10FFFF + 22,
    F13               = 0x10FFFF + 23,
    F14               = 0x10FFFF + 24,
    F15               = 0x10FFFF + 25,
    F16               = 0x10FFFF + 26,
    F17               = 0x10FFFF + 27,
    F18               = 0x10FFFF + 28,
    F19               = 0x10FFFF + 29,
    F20               = 0x10FFFF + 30,
    F21               = 0x10FFFF + 31,
    F22               = 0x10FFFF + 32,
    F23               = 0x10FFFF + 33,
    F24               = 0x10FFFF + 34,
    PrintScreen       = 0x10FFFF + 35,
    Menu              = 0x10FFFF + 36,
  };

  Key() = default;
  explicit Key(const Term::Key::Value& value);
  // clang-format off
  operator Term::Key::Value() const;
  // clang-format on

  bool iscntrl() const;
  bool isblank() const;
  bool isspace() const;
  bool isupper() const;
  bool islower() const;
  bool isalpha() const;
  bool isdigit() const;
  bool isxdigit() const;
  bool isalnum() const;
  bool ispunct() const;
  bool isgraph() const;
  bool isprint() const;

  bool isunicode() const;

  char tolower();
  char toupper();

  // Detect if Key is convertible to ANSII
  bool        isASCII() const;
  // Detect if Key is convertible to Extended ANSII
  bool        isExtendedASCII() const;
  // Detect if Key has CTRL+* (excluding the CTRL+* that can be access with standard key Tab Backspace Enter...)
  bool        hasCtrl() const;
  // Detect if Key has CTRL+*
  bool        hasCtrlAll() const;  //FIXME Find a good name for it
  // Detect if Key has ALT+*
  bool        hasAlt() const;
  bool        empty() const;
  std::string name() const;

  std::string str() const;

private:
  Value m_value{NoKey};
};

class MetaKey
{
public:
  enum Value : std::int32_t
  {
    // Last utf8 codepoint is U+10FFFF (000100001111111111111111) So:
    None = 0,
    Alt  = (1 << 22),
    Ctrl = (1 << 23),
  };
  MetaKey() = default;
  MetaKey(const MetaKey::Value& value);
  Term::MetaKey  operator+(const MetaKey& meta) const;
  Term::Key      operator+(const Key& key) const;
  Term::MetaKey& operator+=(const MetaKey& meta);
  bool           operator==(const Term::MetaKey& meta) const;
  bool           operator!=(const Term::MetaKey& meta) const;
  bool           hasAlt() const;
  bool           hasCtrl() const;

private:
  MetaKey::Value m_value{Value::None};
};

// Preempt them
static const Term::MetaKey Ctrl{Term::MetaKey(Term::MetaKey::Value::Ctrl)};
static const Term::MetaKey Alt{Term::MetaKey(Term::MetaKey::Value::Alt)};

}  // namespace Term
