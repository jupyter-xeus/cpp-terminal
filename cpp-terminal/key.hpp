#pragma once

#include <cstdint>
#include <string>

#include "cpp-terminal/platforms/conversion.hpp"

namespace Term
{

#if __cplusplus >= 201703L
  #define TERM_CONSTEXPR_VAR constexpr
  #define TERM_CONSTEXPR_FUNCTION constexpr
#else
  #define TERM_CONSTEXPR_VAR const
  #define TERM_CONSTEXPR_FUNCTION 
#endif

class Key{
public:
	using value_type = std::int32_t;

	  static TERM_CONSTEXPR_VAR inline std::int32_t NoKey = -1;
	  // Now use << to for detecting special key + key press

	  // Begin ASCII (some ASCII names has been change to their Ctrl_+key part) the value is different to be able to do
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_Arobase      = 0;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_A            = 1;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_B            = 2;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_C            = 3;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_D            = 4;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_E            = 5;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_F            = 6;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_G            = 7;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_H            = 8;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_I            = 9;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_J            = 10;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_K            = 11;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_L            = 12;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_M            = 13;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_N            = 14;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_O            = 15;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_P            = 16;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_Q            = 17;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_R            = 18;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_S            = 19;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_T            = 20;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_U            = 21;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_V            = 22;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_W            = 23;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_X            = 24;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_Y            = 25;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_Z            = 26;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_OpenBracket  = 27;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_BackSlash    = 28;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_CloseBracket = 29;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_Caret        = 30;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ctrl_Underscore   = 31;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Space             = 32;
	  static TERM_CONSTEXPR_VAR inline std::int32_t ExclamationMark   = 33;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Quote             = 34;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Hah               = 35;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Dollar            = 36;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Percent           = 37;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Ampersand         = 38;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Apostrophe        = 39;
	  static TERM_CONSTEXPR_VAR inline std::int32_t OpenParenthesis   = 40;
	  static TERM_CONSTEXPR_VAR inline std::int32_t CloseParenthesis  = 41;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Asterisk          = 42;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Plus              = 43;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Comma             = 44;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Hyphen            = 45;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Minus             = 45;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Period            = 46;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Slash             = 47;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Zero              = 48;
	  static TERM_CONSTEXPR_VAR inline std::int32_t One               = 49;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Two               = 50;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Three             = 51;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Four              = 52;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Five              = 53;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Six               = 54;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Seven             = 55;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Eight             = 56;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Nine              = 57;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Colon             = 58;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Semicolon         = 59;
	  static TERM_CONSTEXPR_VAR inline std::int32_t LessThan          = 60;
	  static TERM_CONSTEXPR_VAR inline std::int32_t OpenChevron       = 60;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Equal             = 61;
	  static TERM_CONSTEXPR_VAR inline std::int32_t GreaterThan       = 62;
	  static TERM_CONSTEXPR_VAR inline std::int32_t CloseChevron      = 62;
	  static TERM_CONSTEXPR_VAR inline std::int32_t QuestionMark      = 63;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Arobase           = 64;
	  static TERM_CONSTEXPR_VAR inline std::int32_t A                 = 65;
	  static TERM_CONSTEXPR_VAR inline std::int32_t B                 = 66;
	  static TERM_CONSTEXPR_VAR inline std::int32_t C                 = 67;
	  static TERM_CONSTEXPR_VAR inline std::int32_t D                 = 68;
	  static TERM_CONSTEXPR_VAR inline std::int32_t E                 = 69;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F                 = 70;
	  static TERM_CONSTEXPR_VAR inline std::int32_t G                 = 71;
	  static TERM_CONSTEXPR_VAR inline std::int32_t H                 = 72;
	  static TERM_CONSTEXPR_VAR inline std::int32_t I                 = 73;
	  static TERM_CONSTEXPR_VAR inline std::int32_t J                 = 74;
	  static TERM_CONSTEXPR_VAR inline std::int32_t K                 = 75;
	  static TERM_CONSTEXPR_VAR inline std::int32_t L                 = 76;
	  static TERM_CONSTEXPR_VAR inline std::int32_t M                 = 77;
	  static TERM_CONSTEXPR_VAR inline std::int32_t N                 = 78;
	  static TERM_CONSTEXPR_VAR inline std::int32_t O                 = 79;
	  static TERM_CONSTEXPR_VAR inline std::int32_t P                 = 80;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Q                 = 81;
	  static TERM_CONSTEXPR_VAR inline std::int32_t R                 = 82;
	  static TERM_CONSTEXPR_VAR inline std::int32_t S                 = 83;
	  static TERM_CONSTEXPR_VAR inline std::int32_t T                 = 84;
	  static TERM_CONSTEXPR_VAR inline std::int32_t U                 = 85;
	  static TERM_CONSTEXPR_VAR inline std::int32_t V                 = 86;
	  static TERM_CONSTEXPR_VAR inline std::int32_t W                 = 87;
	  static TERM_CONSTEXPR_VAR inline std::int32_t X                 = 88;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Y                 = 89;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Z                 = 90;
	  static TERM_CONSTEXPR_VAR inline std::int32_t OpenBracket       = 91;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Backslash         = 92;
	  static TERM_CONSTEXPR_VAR inline std::int32_t CloseBracket      = 93;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Caret             = 94;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Underscore        = 95;
	  static TERM_CONSTEXPR_VAR inline std::int32_t GraveAccent       = 96;
	  static TERM_CONSTEXPR_VAR inline std::int32_t a                 = 97;
	  static TERM_CONSTEXPR_VAR inline std::int32_t b                 = 98;
	  static TERM_CONSTEXPR_VAR inline std::int32_t c                 = 99;
	  static TERM_CONSTEXPR_VAR inline std::int32_t d                 = 100;
	  static TERM_CONSTEXPR_VAR inline std::int32_t e                 = 101;
	  static TERM_CONSTEXPR_VAR inline std::int32_t f                 = 102;
	  static TERM_CONSTEXPR_VAR inline std::int32_t g                 = 103;
	  static TERM_CONSTEXPR_VAR inline std::int32_t h                 = 104;
	  static TERM_CONSTEXPR_VAR inline std::int32_t i                 = 105;
	  static TERM_CONSTEXPR_VAR inline std::int32_t j                 = 106;
	  static TERM_CONSTEXPR_VAR inline std::int32_t k                 = 107;
	  static TERM_CONSTEXPR_VAR inline std::int32_t l                 = 108;
	  static TERM_CONSTEXPR_VAR inline std::int32_t m                 = 109;
	  static TERM_CONSTEXPR_VAR inline std::int32_t n                 = 110;
	  static TERM_CONSTEXPR_VAR inline std::int32_t o                 = 111;
	  static TERM_CONSTEXPR_VAR inline std::int32_t p                 = 112;
	  static TERM_CONSTEXPR_VAR inline std::int32_t q                 = 113;
	  static TERM_CONSTEXPR_VAR inline std::int32_t r                 = 114;
	  static TERM_CONSTEXPR_VAR inline std::int32_t s                 = 115;
	  static TERM_CONSTEXPR_VAR inline std::int32_t t                 = 116;
	  static TERM_CONSTEXPR_VAR inline std::int32_t u                 = 117;
	  static TERM_CONSTEXPR_VAR inline std::int32_t v                 = 118;
	  static TERM_CONSTEXPR_VAR inline std::int32_t w                 = 119;
	  static TERM_CONSTEXPR_VAR inline std::int32_t x                 = 120;
	  static TERM_CONSTEXPR_VAR inline std::int32_t y                 = 121;
	  static TERM_CONSTEXPR_VAR inline std::int32_t z                 = 122;
	  static TERM_CONSTEXPR_VAR inline std::int32_t OpenBrace         = 123;
	  static TERM_CONSTEXPR_VAR inline std::int32_t VerticalBar       = 124;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Close_Brace       = 125;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Tilde             = 126;
	  static TERM_CONSTEXPR_VAR inline std::int32_t CTRL_QuestionMark = 127;
	  // Very useful CTRL_* alternative names
	  static TERM_CONSTEXPR_VAR inline std::int32_t Null              = 0;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Backspace         = 8;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Tab               = 9;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Enter             = 13;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Esc               = 27;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Del               = 127;
	  //
	  // End ASCII
	  // Extended ASCII goes up to 255
	  // Last Unicode codepage 0x10FFFF
	  static TERM_CONSTEXPR_VAR inline std::int32_t ArrowLeft         = 0x10FFFF + 1;
	  static TERM_CONSTEXPR_VAR inline std::int32_t ArrowRight        = 0x10FFFF + 2;
	  static TERM_CONSTEXPR_VAR inline std::int32_t ArrowUp           = 0x10FFFF + 3;
	  static TERM_CONSTEXPR_VAR inline std::int32_t ArrowDown         = 0x10FFFF + 4;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Numeric5          = 0x10FFFF + 5;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Home              = 0x10FFFF + 6;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Insert            = 0x10FFFF + 7;
	  static TERM_CONSTEXPR_VAR inline std::int32_t End               = 0x10FFFF + 8;
	  static TERM_CONSTEXPR_VAR inline std::int32_t PageUp            = 0x10FFFF + 9;
	  static TERM_CONSTEXPR_VAR inline std::int32_t PageDown          = 0x10FFFF + 10;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F1                = 0x10FFFF + 11;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F2                = 0x10FFFF + 12;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F3                = 0x10FFFF + 13;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F4                = 0x10FFFF + 14;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F5                = 0x10FFFF + 15;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F6                = 0x10FFFF + 16;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F7                = 0x10FFFF + 17;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F8                = 0x10FFFF + 18;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F9                = 0x10FFFF + 19;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F10               = 0x10FFFF + 20;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F11               = 0x10FFFF + 21;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F12               = 0x10FFFF + 22;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F13               = 0x10FFFF + 23;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F14               = 0x10FFFF + 24;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F15               = 0x10FFFF + 25;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F16               = 0x10FFFF + 26;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F17               = 0x10FFFF + 27;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F18               = 0x10FFFF + 28;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F19               = 0x10FFFF + 29;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F20               = 0x10FFFF + 30;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F21               = 0x10FFFF + 31;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F22               = 0x10FFFF + 32;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F23               = 0x10FFFF + 33;
	  static TERM_CONSTEXPR_VAR inline std::int32_t F24               = 0x10FFFF + 34;
	  static TERM_CONSTEXPR_VAR inline std::int32_t PrintScreen       = 0x10FFFF + 35;
	  static TERM_CONSTEXPR_VAR inline std::int32_t Menu              = 0x10FFFF + 36;
	

	// member variable value
	// cannot be Key::Value and has to be std::int32_t because it can also have numbers
	// that are not named within the enum. Otherwise it would be undefined behaviour
	std::int32_t value = static_cast<std::int32_t>(Key::Null); 
	
	TERM_CONSTEXPR_FUNCTION inline Key() = default;
	TERM_CONSTEXPR_FUNCTION inline Key(const Key& key) = default;
	TERM_CONSTEXPR_FUNCTION inline Key& operator=(const Key& key) = default;
	
	//TERM_CONSTEXPR_FUNCTION inline Key(const Value& v) : value(static_cast<std::int32_t>(v)){}
	//TERM_CONSTEXPR_FUNCTION inline Key& operator=(const Value& v){this->value = static_cast<std::int32_t>(v); return *this;}
	
	explicit TERM_CONSTEXPR_FUNCTION inline Key(char val) : value(static_cast<std::int32_t>(val)){}
	TERM_CONSTEXPR_FUNCTION inline Key& operator= (char val) {value = static_cast<std::int32_t>(val); return *this;}
	
	TERM_CONSTEXPR_FUNCTION inline Key(std::int32_t val) : value(val){}
	TERM_CONSTEXPR_FUNCTION inline Key& operator= (std::int32_t val) {value = val; return *this;}
	
	explicit TERM_CONSTEXPR_FUNCTION inline Key(std::size_t val) : value(static_cast<std::int32_t>(val)){}
	TERM_CONSTEXPR_FUNCTION inline Key& operator= (std::size_t val) {value = static_cast<std::int32_t>(val); return *this;}
	
	explicit TERM_CONSTEXPR_FUNCTION inline Key(char32_t val) : value(static_cast<std::int32_t>(val)){}
	TERM_CONSTEXPR_FUNCTION inline Key& operator= (char32_t val) {value = static_cast<std::int32_t>(val); return *this;}
	
	explicit TERM_CONSTEXPR_FUNCTION inline Key(std::string_view str) {(void)Term::utf8_to_utf32(str.begin(), str.end(), &this->value);}
	TERM_CONSTEXPR_FUNCTION inline Key& operator= (std::string_view str) {(void)Term::utf8_to_utf32(str.begin(), str.end(), &this->value); return *this;}
	
	TERM_CONSTEXPR_FUNCTION inline operator std::int32_t (){return this->value;}
};

class MetaKey{
public:
	enum class Value : std::int32_t
	{
	  // Last utf8 codepoint is U+10FFFF (000100001111111111111111) So:
	  None = 0,
	  Alt  = (1 << 22),
	  Ctrl = (1 << 23),
	};
	
	static TERM_CONSTEXPR_VAR inline Value Alt  = MetaKey::Value::Alt;
	static TERM_CONSTEXPR_VAR inline Value Ctrl = MetaKey::Value::Ctrl;
	static TERM_CONSTEXPR_VAR inline Value None = MetaKey::Value::None;
	
	std::int32_t value = static_cast<std::int32_t>(Value::None);
	
	TERM_CONSTEXPR_FUNCTION inline MetaKey() = default;
	TERM_CONSTEXPR_FUNCTION inline MetaKey(const MetaKey& key) = default;
	TERM_CONSTEXPR_FUNCTION inline MetaKey& operator=(const MetaKey& key) = default;
	
	TERM_CONSTEXPR_FUNCTION inline MetaKey(const Value& v) : value(static_cast<std::int32_t>(v)){}
	TERM_CONSTEXPR_FUNCTION inline MetaKey& operator=(const Value& v){this->value = static_cast<std::int32_t>(v); return *this;}
	
	explicit TERM_CONSTEXPR_FUNCTION inline MetaKey(std::int32_t val) : value(val){}
	TERM_CONSTEXPR_FUNCTION inline MetaKey& operator= (std::int32_t val) {value = val; return *this;}
};

TERM_CONSTEXPR_FUNCTION inline bool operator==(Key l, Key r) { return l.value == r.value; }
//TERM_CONSTEXPR_FUNCTION inline bool operator==(Key l, Key::Value r) { return l == Key(r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator==(Key::Value l, Key r) { return Key(l) == r; }
TERM_CONSTEXPR_FUNCTION inline bool operator==(MetaKey l, MetaKey r) { return l.value == r.value; }
TERM_CONSTEXPR_FUNCTION inline bool operator==(MetaKey l, MetaKey::Value r) { return l == MetaKey(r);}
TERM_CONSTEXPR_FUNCTION inline bool operator==(MetaKey::Value l, MetaKey r) { return MetaKey(l) == r;}
TERM_CONSTEXPR_FUNCTION inline bool operator==(Key l, MetaKey r) { return l.value == r.value; }
TERM_CONSTEXPR_FUNCTION inline bool operator==(MetaKey l, Key r) { return l.value == r.value; }
TERM_CONSTEXPR_FUNCTION inline bool operator==(Key l, char r) { return l == Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator==(char l, Key r) { return Key(l) == r; }
//TERM_CONSTEXPR_FUNCTION inline bool operator==(Key::Value l, char r) { return Key(l) == Key(r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator==(char l, Key::Value r) { return Key(l) == Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator==(Key l, char32_t r) { return l == Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator==(char32_t l, Key r) { return Key(l) == r; }
//TERM_CONSTEXPR_FUNCTION inline bool operator==(Key::Value l, char32_t r) { return Key(l) == Key(r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator==(char32_t l, Key::Value r) { return Key(l) == Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator==(Key l, std::int32_t r) { return l == Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator==(std::int32_t l, Key r) { return Key(l) == r; }
TERM_CONSTEXPR_FUNCTION inline bool operator==(Key l, std::size_t r) { return static_cast<std::size_t>(l.value) == r; }
TERM_CONSTEXPR_FUNCTION inline bool operator==(std::size_t l, Key r) { return l == static_cast<std::size_t>(r.value); }
//TERM_CONSTEXPR_FUNCTION inline bool operator==(Key::Value l, std::int32_t r) { return Key(l) == Key(r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator==(std::int32_t l, Key::Value r) { return Key(l) == Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator==(Key l, std::string_view r) { 
	if(r.size() == 0) return l == Key::Null;
	else return (r.size() == static_cast<size_t>(Term::Private::identify(r.front()))) && (l == Key(r)); 
}
TERM_CONSTEXPR_FUNCTION inline bool operator==(std::string_view l, Key r) {return r == l;}
//TERM_CONSTEXPR_FUNCTION inline bool operator==(Key::Value& l, std::string_view r) { return Key(l) == Key(r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator==(std::string_view l, Key::Value r) { return Key(l) == Key(r); }


TERM_CONSTEXPR_FUNCTION inline bool operator<(Key l, Key r) { return l.value < r.value; }
//TERM_CONSTEXPR_FUNCTION inline bool operator<(Key l, Key::Value r) { return l < Key(r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<(Key::Value l, Key r) { return Key(l) < r; }
TERM_CONSTEXPR_FUNCTION inline bool operator<(MetaKey l, MetaKey r) 			{ return l.value < r.value; }
//TERM_CONSTEXPR_FUNCTION inline bool operator<(MetaKey l, MetaKey::Value r)	{ return l < MetaKey(r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<(MetaKey::Value l, MetaKey r)	{ return MetaKey(l) < r; }
TERM_CONSTEXPR_FUNCTION inline bool operator<(MetaKey l, Key r) 				{ return l.value < r.value; }
TERM_CONSTEXPR_FUNCTION inline bool operator<(Key l, MetaKey r) 				{ return l.value < r.value; }
TERM_CONSTEXPR_FUNCTION inline bool operator<(Key l, char r) { return l < Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<(char l, Key r) { return Key(l) < r; }
//TERM_CONSTEXPR_FUNCTION inline bool operator<(Key::Value l, char r) { return Key(l) < Key(r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<(char l, Key::Value r) { return Key(l) < Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<(Key l, char32_t r) { return l < Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<(char32_t l, Key r) { return Key(l) < r; }
//TERM_CONSTEXPR_FUNCTION inline bool operator<(Key::Value l, char32_t r) { return Key(l) < Key(r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<(char32_t l, Key::Value r) { return Key(l) < Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<(Key l, std::int32_t r) { return l < Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<(std::int32_t l, Key r) { return Key(l) < r; }
TERM_CONSTEXPR_FUNCTION inline bool operator<(Key l, std::size_t r) { return static_cast<std::size_t>(l.value) < r; }
TERM_CONSTEXPR_FUNCTION inline bool operator<(std::size_t l, Key r) { return l < static_cast<std::size_t>(r.value); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<(Key::Value l, std::int32_t r) { return Key(l) < Key(r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<(std::int32_t l, Key::Value r) { return Key(l) < Key(r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<(Key l, std::string_view r) { 
	if(r.size() == 0) return l != Key::Null;
	else return (r.size() == static_cast<size_t>(Term::Private::identify(r.front()))) && (l < Key(r)); 
}
TERM_CONSTEXPR_FUNCTION inline bool operator<(std::string_view l, Key r) { 
	if(l.size() == 0) return r != Key::Null;
	else return (l.size() == static_cast<size_t>(Term::Private::identify(l.front()))) && (Key(l) < r); 
}
//TERM_CONSTEXPR_FUNCTION inline bool operator<(Key::Value& l, std::string_view r) { return Key(l) < Key(r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<(std::string_view l, Key::Value r) { return Key(l) < Key(r); }


TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key l, Key r) 						{ return !(l == r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key l, Key::Value r) 					{ return !(l == r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key::Value l, Key r) 					{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(MetaKey l, MetaKey r) 				{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(MetaKey l, MetaKey::Value r)			{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(MetaKey::Value l, MetaKey r)			{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key l, MetaKey r) 					{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(MetaKey l, Key r) 					{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key l, char r) 						{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(char l, Key r) 						{ return !(l == r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key::Value l, char r) 				{ return !(l == r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator!=(char l, Key::Value r) 				{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key l, char32_t r) 					{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(char32_t l, Key r) 					{ return !(l == r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key::Value l, char32_t r) 			{ return !(l == r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator!=(char32_t l, Key::Value r) 			{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key l, std::int32_t r) 				{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(std::int32_t l, Key r) 				{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key l, std::size_t r) 				{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(std::size_t l, Key r) 				{ return !(l == r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key::Value l, std::int32_t r) 		{ return !(l == r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator!=(std::int32_t l, Key::Value r) 		{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key l, std::string_view r) 			{ return !(l == r); }
TERM_CONSTEXPR_FUNCTION inline bool operator!=(std::string_view l, Key r) 			{ return !(l == r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator!=(Key::Value& l, std::string_view r) 	{ return !(l == r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator!=(std::string_view l, Key::Value r) 	{ return !(l == r); }

TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key l, Key r) 						{ return !(r < l); }
//TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key l, Key::Value r) 					{ return !(r < l); }
//TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key::Value l, Key r) 					{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(MetaKey l, MetaKey r) 				{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(MetaKey l, MetaKey::Value r)			{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(MetaKey::Value l, MetaKey r)			{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(MetaKey l, Key r) 					{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key l, MetaKey r) 					{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key l, char r) 						{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(char l, Key r) 						{ return !(r < l); }
//TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key::Value l, char r) 				{ return !(r < l); }
//TERM_CONSTEXPR_FUNCTION inline bool operator>=(char l, Key::Value r) 				{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key l, char32_t r) 					{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(char32_t l, Key r) 					{ return !(r < l); }
//TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key::Value l, char32_t r) 			{ return !(r < l); }
//TERM_CONSTEXPR_FUNCTION inline bool operator>=(char32_t l, Key::Value r) 			{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key l, std::int32_t r) 				{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(std::int32_t l, Key r) 				{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key l, std::size_t r) 				{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(std::size_t l, Key r) 				{ return !(r < l); }
//TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key::Value l, std::int32_t r) 		{ return !(r < l); }
//TERM_CONSTEXPR_FUNCTION inline bool operator>=(std::int32_t l, Key::Value r) 		{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key l, std::string_view r) 			{ return !(r < l); }
TERM_CONSTEXPR_FUNCTION inline bool operator>=(std::string_view l, Key r) 			{ return !(r < l); }
//TERM_CONSTEXPR_FUNCTION inline bool operator>=(Key::Value& l, std::string_view r) 	{ return !(r < l); }
//TERM_CONSTEXPR_FUNCTION inline bool operator>=(std::string_view l, Key::Value r) 	{ return !(r < l); }

TERM_CONSTEXPR_FUNCTION inline bool operator>(Key l, Key r) 							{ return r < l; }
//TERM_CONSTEXPR_FUNCTION inline bool operator>(Key l, Key::Value r) 					{ return r < l; }
//TERM_CONSTEXPR_FUNCTION inline bool operator>(Key::Value l, Key r) 					{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(MetaKey l, MetaKey r) 					{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(MetaKey l, MetaKey::Value r)			{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(MetaKey::Value l, MetaKey r)			{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(MetaKey l, Key r) 						{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(Key l, MetaKey r) 						{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(Key l, char r) 						{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(char l, Key r) 						{ return r < l; }
//TERM_CONSTEXPR_FUNCTION inline bool operator>(Key::Value l, char r) 					{ return r < l; }
//TERM_CONSTEXPR_FUNCTION inline bool operator>(char l, Key::Value r) 					{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(Key l, char32_t r) 					{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(char32_t l, Key r) 					{ return r < l; }
//TERM_CONSTEXPR_FUNCTION inline bool operator>(Key::Value l, char32_t r) 				{ return r < l; }
//TERM_CONSTEXPR_FUNCTION inline bool operator>(char32_t l, Key::Value r) 				{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(Key l, std::int32_t r) 				{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(std::int32_t l, Key r) 				{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(Key l, std::size_t r) 				{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(std::size_t l, Key r) 				{ return r < l; }
//TERM_CONSTEXPR_FUNCTION inline bool operator>(Key::Value l, std::int32_t r) 			{ return r < l; }
//TERM_CONSTEXPR_FUNCTION inline bool operator>(std::int32_t l, Key::Value r) 			{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(Key l, std::string_view r) 			{ return r < l; }
TERM_CONSTEXPR_FUNCTION inline bool operator>(std::string_view l, Key r) 			{ return r < l; }
//TERM_CONSTEXPR_FUNCTION inline bool operator>(Key::Value& l, std::string_view r) 	{ return r < l; }
//TERM_CONSTEXPR_FUNCTION inline bool operator>(std::string_view l, Key::Value r) 		{ return r < l; }

TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key l, Key r) 						{ return !(l > r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key l, Key::Value r) 					{ return !(l > r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key::Value l, Key r) 					{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(MetaKey l, MetaKey r) 				{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(MetaKey l, MetaKey::Value r)			{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(MetaKey::Value l, MetaKey r)			{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(MetaKey l, Key r) 					{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key l, MetaKey r) 					{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key l, char r) 						{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(char l, Key r) 						{ return !(l > r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key::Value l, char r) 				{ return !(l > r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<=(char l, Key::Value r) 				{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key l, char32_t r) 					{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(char32_t l, Key r) 					{ return !(l > r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key::Value l, char32_t r) 			{ return !(l > r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<=(char32_t l, Key::Value r) 			{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key l, std::int32_t r) 				{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(std::int32_t l, Key r) 				{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key l, std::size_t r) 				{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(std::size_t l, Key r) 				{ return !(l > r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key::Value l, std::int32_t r) 		{ return !(l > r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<=(std::int32_t l, Key::Value r) 		{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key l, std::string_view r) 			{ return !(l > r); }
TERM_CONSTEXPR_FUNCTION inline bool operator<=(std::string_view l, Key r) 			{ return !(l > r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<=(Key::Value& l, std::string_view r) 	{ return !(l > r); }
//TERM_CONSTEXPR_FUNCTION inline bool operator<=(std::string_view l, Key::Value r) 	{ return !(l > r); }

TERM_CONSTEXPR_FUNCTION inline bool iscntrl(Key key) { return (key >= Key::Null && key <= Key::Ctrl_Underscore) || key == Key::Del; }
TERM_CONSTEXPR_FUNCTION inline bool isblank(Key key) { return key == Key::Tab || key == Key::Space; }
TERM_CONSTEXPR_FUNCTION inline bool isspace(Key key) { return isblank(key) || (key >= Key::Ctrl_J && key <= Key::Enter); }
TERM_CONSTEXPR_FUNCTION inline bool isupper(Key key) { return key >= Key::A && key <= Key::Z; }
TERM_CONSTEXPR_FUNCTION inline bool islower(Key key) { return key >= Key::a && key <= Key::z; }
TERM_CONSTEXPR_FUNCTION inline bool isalpha(Key key) { return (isupper(key) || islower(key)); }
TERM_CONSTEXPR_FUNCTION inline bool isdigit(Key key) { return key >= Key::Zero && key <= Key::Nine; }
TERM_CONSTEXPR_FUNCTION inline bool isxdigit(Key key) { return isdigit(key) || (key >= Key::A && key <= Key::F) || (key >= Key::a && key <= Key::f); }
TERM_CONSTEXPR_FUNCTION inline bool isalnum(Key key) { return (isdigit(key) || isalpha(key)); }
TERM_CONSTEXPR_FUNCTION inline bool ispunct(Key key) { return (key >= Key::ExclamationMark && key <= Key::Slash) || (key >= Key::Colon && key <= Key::Arobase) || (key >= Key::OpenBracket && key <= Key::GraveAccent) || (key >= Key::OpenBrace && key <= Key::Tilde); }
TERM_CONSTEXPR_FUNCTION inline bool isgraph(Key key) { return (isalnum(key) || ispunct(key)); }
TERM_CONSTEXPR_FUNCTION inline bool isprint(Key key) { return (isgraph(key) || key == Key::Space); }
TERM_CONSTEXPR_FUNCTION inline bool isunicode(Key key){ return key >= Key::Null && key.value <= 0x10FFFFL;}
TERM_CONSTEXPR_FUNCTION inline Key tolower(Key key) { return (isalpha(key) && isupper(key)) ? Key(key.value + 32) : key; }
TERM_CONSTEXPR_FUNCTION inline Key toupper(Key key) { return (isalpha(key) && islower(key)) ? Key(key.value - 32) : key; }

// Detect if key is convertible to ANSII
TERM_CONSTEXPR_FUNCTION inline bool isASCII(Key key) { return key >= Key::Null && key <= Key::Del; }

// Detect if key is convertible to Extended ANSII
TERM_CONSTEXPR_FUNCTION inline bool isExtendedASCII(Key key) { return key >= Key::Null && key.value <= 255L; }

// Detect if key has CTRL+*
TERM_CONSTEXPR_FUNCTION inline bool hasCtrlAll(Key key) { return iscntrl(key) || ((key.value & static_cast<std::int32_t>(MetaKey::Value::Ctrl)) == static_cast<std::int32_t>(MetaKey::Value::Ctrl)); }

// Detect if key has CTRL+* (excluding the CTRL+* that can be access with standard key Tab Backspace Enter...)
TERM_CONSTEXPR_FUNCTION inline bool hasCtrl(Key key)
{
  // Need to suppress the TAB etc...
  return ((iscntrl(key) || hasCtrlAll(key)) && key != Key::Backspace && key != Key::Tab && key != Key::Esc && key != Key::Enter && key != Key::Del);
}

// Detect if key has ALT+*
TERM_CONSTEXPR_FUNCTION inline bool hasAlt(Key key) { return (key.value & static_cast<std::int32_t>(MetaKey::Value::Alt)) == static_cast<std::int32_t>(MetaKey::Value::Alt); }

TERM_CONSTEXPR_FUNCTION inline bool empty(Key key) { return (key == Key::NoKey); }

void               append_name(Key key, std::string& strOut);
void               append_str(Key key, std::string& strOut);
inline std::string name(Key key)
{
  std::string str;
  append_name(key, str);
  return str;
}

inline std::string str(Key key)
{
  std::string str;
  append_str(key, str);
  return str;
}

TERM_CONSTEXPR_FUNCTION inline bool hasAlt(MetaKey metakey) { return (metakey.value & static_cast<std::int32_t>(MetaKey::Value::Alt)) == static_cast<std::int32_t>(MetaKey::Value::Alt); }
TERM_CONSTEXPR_FUNCTION inline bool hasCtrl(MetaKey metakey) { return (metakey.value & static_cast<std::int32_t>(MetaKey::Value::Ctrl)) == static_cast<std::int32_t>(MetaKey::Value::Ctrl); }

TERM_CONSTEXPR_FUNCTION inline MetaKey operator+(MetaKey l, MetaKey r) { return MetaKey(l.value | r.value); }
TERM_CONSTEXPR_FUNCTION inline Key operator+(MetaKey metakey, Key key)
{
  if(empty(key)) {
	return key;
  }else if(metakey == MetaKey::Value::None){
	 return key;
  }
  
  // two seperate 'if' because metakey could be both at the same time 
  if(metakey == MetaKey::Value::Ctrl){
	  if(hasCtrlAll(key)) return key;
      else
        key = Key(key.value + static_cast<std::int32_t>(MetaKey::Value::Ctrl));  // FIXME maybe a better check;
  }
  
  if(metakey == MetaKey::Value::Alt){
      if(hasAlt(key)) return key;
      else
        key = Key(key.value + static_cast<std::int32_t>(MetaKey::Value::Alt));  // FIXME maybe a better check;
  }
  
  return key;
}
TERM_CONSTEXPR_FUNCTION inline Key operator+(Key key, MetaKey meta) { return meta + key; }
TERM_CONSTEXPR_FUNCTION inline MetaKey operator+(MetaKey::Value l, MetaKey::Value r) {return MetaKey(l) + MetaKey(r);}
TERM_CONSTEXPR_FUNCTION inline Key operator+(MetaKey::Value l, Key r) {return MetaKey(l) + r;}
TERM_CONSTEXPR_FUNCTION inline Key operator+(Key l, MetaKey::Value r) {return l + MetaKey(r);}
TERM_CONSTEXPR_FUNCTION inline Key operator+(MetaKey::Value l, Key::value_type r) {return MetaKey(l) + Key(r);}
TERM_CONSTEXPR_FUNCTION inline Key operator+(Key::value_type l, MetaKey::Value r) {return Key(l) + MetaKey(r);}
 

}  // namespace Term

#undef TERM_CONSTEXPR_FUNCTION
#undef TERM_CONSTEXPR_VAR
