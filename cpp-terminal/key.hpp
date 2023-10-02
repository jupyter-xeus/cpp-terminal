/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/platforms/conversion.hpp"

#include <cstdint>
#include <string>

namespace Term
{

class MetaKey
{
public:
  enum class Value : std::int32_t
  {
    // Last utf8 codepoint is U+10FFFF (000100001111111111111111) So:
    None = 0,
    Alt  = (1 << 22),
    Ctrl = (1 << 23),
  };

  constexpr MetaKey() : value(static_cast<std::int32_t>(Value::None)) {}
  constexpr MetaKey(const MetaKey& key)         = default;
  inline MetaKey& operator=(const MetaKey& key) = default;

  constexpr MetaKey(const Value& v) : value(static_cast<std::int32_t>(v)) {}
  inline MetaKey& operator=(const Value& v)
  {
    this->value = static_cast<std::int32_t>(v);
    return *this;
  }

  explicit constexpr MetaKey(std::int32_t val) : value(val) {}
  inline MetaKey& operator=(std::int32_t val)
  {
    this->value = val;
    return *this;
  }

  explicit constexpr operator std::int32_t() const { return this->value; }

  constexpr bool hasAlt() const { return (this->value & static_cast<std::int32_t>(MetaKey::Value::Alt)) == static_cast<std::int32_t>(MetaKey::Value::Alt); }
  constexpr bool hasCtrl() const { return (this->value & static_cast<std::int32_t>(MetaKey::Value::Ctrl)) == static_cast<std::int32_t>(MetaKey::Value::Ctrl); }

  friend constexpr MetaKey operator+(MetaKey l, MetaKey r) { return MetaKey(l.value | r.value); }
  friend constexpr MetaKey operator+(MetaKey::Value l, MetaKey::Value r) { return MetaKey(l) + MetaKey(r); }
  friend constexpr MetaKey operator+(MetaKey l, MetaKey::Value r) { return l + MetaKey(r); }
  friend constexpr MetaKey operator+(MetaKey::Value l, MetaKey r) { return MetaKey(l) + r; }

  MetaKey& operator+=(MetaKey r) { return *this = *this + r; }
  MetaKey& operator+=(MetaKey::Value r) { return *this = *this + r; }

  friend constexpr bool operator==(MetaKey l, MetaKey r) { return l.value == r.value; }
  friend constexpr bool operator==(MetaKey l, MetaKey::Value r) { return l == MetaKey(r); }
  friend constexpr bool operator==(MetaKey::Value l, MetaKey r) { return MetaKey(l) == r; }

  friend constexpr bool operator!=(MetaKey l, MetaKey r) { return !(l == r); }
  friend constexpr bool operator!=(MetaKey l, MetaKey::Value r) { return !(l == r); }
  friend constexpr bool operator!=(MetaKey::Value l, MetaKey r) { return !(l == r); }

  friend constexpr bool operator<(MetaKey l, MetaKey r) { return l.value < r.value; }

  friend constexpr bool operator>=(MetaKey l, MetaKey r) { return !(l < r); }
  friend constexpr bool operator>=(MetaKey l, MetaKey::Value r) { return !(l < r); }
  friend constexpr bool operator>=(MetaKey::Value l, MetaKey r) { return !(l < r); }

  friend constexpr bool operator>(MetaKey l, MetaKey r) { return r < l; }
  friend constexpr bool operator>(MetaKey l, MetaKey::Value r) { return r < l; }
  friend constexpr bool operator>(MetaKey::Value l, MetaKey r) { return r < l; }

  friend constexpr bool operator<=(MetaKey l, MetaKey r) { return !(l > r); }
  friend constexpr bool operator<=(MetaKey l, MetaKey::Value r) { return !(l > r); }
  friend constexpr bool operator<=(MetaKey::Value l, MetaKey r) { return !(l > r); }

  std::int32_t value;
};

class Key
{
public:
  using value_type = std::int32_t;

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
    Ctrl_H            = 8, /*corresponds to Backspace*/
    Ctrl_I            = 9, /*corresponds to Tab*/
    Ctrl_J            = 10,
    Ctrl_K            = 11,
    Ctrl_L            = 12,
    Ctrl_M            = 13, /*corresponds to Enter*/
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
    Ctrl_OpenBracket  = 27, /*corresponds to Escape*/
    Ctrl_BackSlash    = 28,
    Ctrl_CloseBracket = 29,
    Ctrl_Caret        = 30,
    Ctrl_Underscore   = 31,
    Space             = 32,
    ExclamationMark   = 33,
    Quote             = 34,
    Hash              = 35,
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
    CTRL_QuestionMark = 127, /*corresponds to DEL*/
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

  constexpr Key() : value(NoKey) {}
  constexpr Key(const Key& key)         = default;
  inline Key& operator=(const Key& key) = default;

  constexpr Key(const Value& v) : value(static_cast<std::int32_t>(v)) {}
  inline Key& operator=(const Value& v)
  {
    this->value = static_cast<std::int32_t>(v);
    return *this;
  }

  explicit constexpr Key(char val) : value(static_cast<std::int32_t>(val)) {}
  inline Key& operator=(char val)
  {
    value = static_cast<std::int32_t>(val);
    return *this;
  }

  constexpr Key(std::int32_t val) : value(val) {}
  inline Key& operator=(std::int32_t val)
  {
    value = val;
    return *this;
  }

  explicit constexpr Key(std::size_t val) : value(static_cast<std::int32_t>(val)) {}
  inline Key& operator=(std::size_t val)
  {
    value = static_cast<std::int32_t>(val);
    return *this;
  }

  explicit constexpr Key(char32_t val) : value(static_cast<std::int32_t>(val)) {}
  inline Key& operator=(char32_t val)
  {
    value = static_cast<std::int32_t>(val);
    return *this;
  }

  constexpr operator std::int32_t() const { return this->value; }

  friend constexpr bool operator==(Key l, Key r) { return l.value == r.value; }
  friend constexpr bool operator==(Key l, char r) { return l == Key(r); }
  friend constexpr bool operator==(char l, Key r) { return Key(l) == r; }
  friend constexpr bool operator==(Key l, char32_t r) { return l == Key(r); }
  friend constexpr bool operator==(char32_t l, Key r) { return Key(l) == r; }
  friend constexpr bool operator==(Key l, std::int32_t r) { return l == Key(r); }
  friend constexpr bool operator==(std::int32_t l, Key r) { return Key(l) == r; }
  friend constexpr bool operator==(Key l, std::size_t r) { return static_cast<std::size_t>(l.value) == r; }
  friend constexpr bool operator==(std::size_t l, Key r) { return l == static_cast<std::size_t>(r.value); }

  friend constexpr bool operator!=(Key l, Key r) { return !(l == r); }
  friend constexpr bool operator!=(Key l, char r) { return !(l == r); }
  friend constexpr bool operator!=(char l, Key r) { return !(l == r); }
  friend constexpr bool operator!=(Key l, char32_t r) { return !(l == r); }
  friend constexpr bool operator!=(char32_t l, Key r) { return !(l == r); }
  friend constexpr bool operator!=(Key l, std::int32_t r) { return !(l == r); }
  friend constexpr bool operator!=(std::int32_t l, Key r) { return !(l == r); }
  friend constexpr bool operator!=(Key l, std::size_t r) { return !(l == r); }
  friend constexpr bool operator!=(std::size_t l, Key r) { return !(l == r); }

  friend constexpr bool operator<(Key l, Key r) { return l.value < r.value; }
  friend constexpr bool operator<(Key l, char r) { return l < Key(r); }
  friend constexpr bool operator<(char l, Key r) { return Key(l) < r; }
  friend constexpr bool operator<(Key l, char32_t r) { return l < Key(r); }
  friend constexpr bool operator<(char32_t l, Key r) { return Key(l) < r; }
  friend constexpr bool operator<(Key l, std::int32_t r) { return l < Key(r); }
  friend constexpr bool operator<(std::int32_t l, Key r) { return Key(l) < r; }
  friend constexpr bool operator<(Key l, std::size_t r) { return static_cast<std::size_t>(l.value) < r; }
  friend constexpr bool operator<(std::size_t l, Key r) { return l < static_cast<std::size_t>(r.value); }

  friend constexpr bool operator>=(Key l, Key r) { return !(l < r); }
  friend constexpr bool operator>=(Key l, char r) { return !(l < r); }
  friend constexpr bool operator>=(char l, Key r) { return !(l < r); }
  friend constexpr bool operator>=(Key l, char32_t r) { return !(l < r); }
  friend constexpr bool operator>=(char32_t l, Key r) { return !(l < r); }
  friend constexpr bool operator>=(Key l, std::int32_t r) { return !(l < r); }
  friend constexpr bool operator>=(std::int32_t l, Key r) { return !(l < r); }
  friend constexpr bool operator>=(Key l, std::size_t r) { return !(l < r); }
  friend constexpr bool operator>=(std::size_t l, Key r) { return !(l < r); }

  friend constexpr bool operator>(Key l, Key r) { return r < l; }
  friend constexpr bool operator>(Key l, char r) { return r < l; }
  friend constexpr bool operator>(char l, Key r) { return r < l; }
  friend constexpr bool operator>(Key l, char32_t r) { return r < l; }
  friend constexpr bool operator>(char32_t l, Key r) { return r < l; }
  friend constexpr bool operator>(Key l, std::int32_t r) { return r < l; }
  friend constexpr bool operator>(std::int32_t l, Key r) { return r < l; }
  friend constexpr bool operator>(Key l, std::size_t r) { return r < l; }
  friend constexpr bool operator>(std::size_t l, Key r) { return r < l; }

  friend constexpr bool operator<=(Key l, Key r) { return !(l > r); }
  friend constexpr bool operator<=(Key l, char r) { return !(l > r); }
  friend constexpr bool operator<=(char l, Key r) { return !(l > r); }
  friend constexpr bool operator<=(Key l, char32_t r) { return !(l > r); }
  friend constexpr bool operator<=(char32_t l, Key r) { return !(l > r); }
  friend constexpr bool operator<=(Key l, std::int32_t r) { return !(l > r); }
  friend constexpr bool operator<=(std::int32_t l, Key r) { return !(l > r); }
  friend constexpr bool operator<=(Key l, std::size_t r) { return !(l > r); }
  friend constexpr bool operator<=(std::size_t l, Key r) { return !(l > r); }

  constexpr bool iscntrl() const { return (*this >= Key::Null && *this <= Key::Ctrl_Underscore) || *this == Key::Del; }
  constexpr bool isblank() const { return *this == Key::Tab || *this == Key::Space; }
  constexpr bool isspace() const { return this->isblank() || (*this >= Key::Ctrl_J && *this <= Key::Enter); }
  constexpr bool isupper() const { return *this >= Key::A && *this <= Key::Z; }
  constexpr bool islower() const { return *this >= Key::a && *this <= Key::z; }
  constexpr bool isalpha() const { return (this->isupper() || this->islower()); }
  constexpr bool isdigit() const { return *this >= Key::Zero && *this <= Key::Nine; }
  constexpr bool isxdigit() const { return this->isdigit() || (*this >= Key::A && *this <= Key::F) || (*this >= Key::a && *this <= Key::f); }
  constexpr bool isalnum() const { return (this->isdigit() || this->isalpha()); }
  constexpr bool ispunct() const { return (*this >= Key::ExclamationMark && *this <= Key::Slash) || (*this >= Key::Colon && *this <= Key::Arobase) || (*this >= Key::OpenBracket && *this <= Key::GraveAccent) || (*this >= Key::OpenBrace && *this <= Key::Tilde); }
  constexpr bool isgraph() const { return (this->isalnum() || this->ispunct()); }
  constexpr bool isprint() const { return (this->isgraph() || *this == Key::Space); }
  constexpr bool isunicode() const { return *this >= Key::Null && this->value <= 0x10FFFFL; }
  constexpr Key  tolower() const { return (this->isalpha() && this->isupper()) ? Key(this->value + 32) : *this; }
  constexpr Key  toupper() const { return (this->isalpha() && this->islower()) ? Key(this->value - 32) : *this; }

  // Detect if *this is convertible to ANSII
  constexpr bool isASCII() const { return *this >= Key::Null && *this <= Key::Del; }

  // Detect if *this is convertible to Extended ANSII
  constexpr bool isExtendedASCII() const { return *this >= Key::Null && this->value <= 255L; }

  // Detect if *this has CTRL+*
  constexpr bool hasCtrlAll() const { return this->iscntrl() || ((this->value & static_cast<std::int32_t>(MetaKey::Value::Ctrl)) == static_cast<std::int32_t>(MetaKey::Value::Ctrl)); }

  // Detect if *this has CTRL+* (excluding the CTRL+* that can be access with standard *this Tab Backspace Enter...)
  constexpr bool hasCtrl() const
  {
    // Need to suppress the TAB etc...
    return ((this->iscntrl() || this->hasCtrlAll()) && *this != Key::Backspace && *this != Key::Tab && *this != Key::Esc && *this != Key::Enter && *this != Key::Del);
  }

  // Detect if key has ALT+*
  constexpr bool hasAlt() const { return (this->value & static_cast<std::int32_t>(MetaKey::Value::Alt)) == static_cast<std::int32_t>(MetaKey::Value::Alt); }

  constexpr bool empty() const { return (this->value == Key::NoKey); }

  void        append_name(std::string& strOut) const;
  void        append_str(std::string& strOut) const;
  std::string name() const;
  std::string str() const;

  // member variable value
  // cannot be Key::Value and has to be std::int32_t because it can also have numbers
  // that are not named within the enum. Otherwise it would be undefined behaviour
  std::int32_t value;
};

constexpr bool operator==(Key l, MetaKey r) { return static_cast<std::int32_t>(l) == static_cast<std::int32_t>(r); }
constexpr bool operator==(MetaKey l, Key r) { return static_cast<std::int32_t>(l) == static_cast<std::int32_t>(r); }

constexpr bool operator<(MetaKey l, Key r) { return static_cast<std::int32_t>(l) < static_cast<std::int32_t>(r); }
constexpr bool operator<(Key l, MetaKey r) { return static_cast<std::int32_t>(l) < static_cast<std::int32_t>(r); }

constexpr bool operator!=(Key l, MetaKey r) { return static_cast<std::int32_t>(l) != static_cast<std::int32_t>(r); }
constexpr bool operator!=(MetaKey l, Key r) { return static_cast<std::int32_t>(l) != static_cast<std::int32_t>(r); }

constexpr bool operator>=(MetaKey l, Key r) { return static_cast<std::int32_t>(l) >= static_cast<std::int32_t>(r); }
constexpr bool operator>=(Key l, MetaKey r) { return static_cast<std::int32_t>(l) >= static_cast<std::int32_t>(r); }

constexpr bool operator>(MetaKey l, Key r) { return static_cast<std::int32_t>(l) > static_cast<std::int32_t>(r); }
constexpr bool operator>(Key l, MetaKey r) { return static_cast<std::int32_t>(l) > static_cast<std::int32_t>(r); }

constexpr bool operator<=(MetaKey l, Key r) { return static_cast<std::int32_t>(l) <= static_cast<std::int32_t>(r); }
constexpr bool operator<=(Key l, MetaKey r) { return static_cast<std::int32_t>(l) <= static_cast<std::int32_t>(r); }

constexpr Key operator+(MetaKey metakey, Key key) { return Key(key.value + ((metakey == MetaKey::Value::Ctrl && !key.hasCtrlAll() && !key.empty()) ? static_cast<std::int32_t>(MetaKey::Value::Ctrl) : 0) + ((metakey == MetaKey::Value::Alt && !key.hasAlt() && !key.empty()) ? static_cast<std::int32_t>(MetaKey::Value::Alt) : 0)); }
constexpr Key operator+(Key key, MetaKey meta) { return meta + key; }

constexpr Key operator+(MetaKey::Value l, Key r) { return MetaKey(l) + r; }
constexpr Key operator+(Key l, MetaKey::Value r) { return l + MetaKey(r); }
constexpr Key operator+(MetaKey::Value l, Key::value_type r) { return MetaKey(l) + Key(r); }
constexpr Key operator+(Key::value_type l, MetaKey::Value r) { return Key(l) + MetaKey(r); }

}  // namespace Term
