#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <cpp-terminal/tools.hpp>

TEST_CASE("UTF conversions") {
    char32_t codepoint = U'\u0001';
    std::string s;
    Term::codepoint_to_utf8(s, codepoint);
    CHECK(s.size() == 1);
    CHECK(s[0] == '\x01');
    CHECK(s == u8"\u0001");

    s = "";
    codepoint = U'\u0080';
    Term::codepoint_to_utf8(s, codepoint);
    CHECK(s.size() == 2);
    CHECK(s[0] == '\xC2');
    CHECK(s[1] == '\x80');
    CHECK(s == u8"\u0080");

    s = "";
    codepoint = U'\u0800';
    Term::codepoint_to_utf8(s, codepoint);
    CHECK(s.size() == 3);
    CHECK(s[0] == '\xE0');
    CHECK(s[1] == '\xA0');
    CHECK(s[2] == '\x80');
    CHECK(s == u8"\u0800");

    s = "";
    codepoint = U'\U0010FFFF';
    Term::codepoint_to_utf8(s, codepoint);
    CHECK(s.size() == 4);
    CHECK(s[0] == '\xF4');
    CHECK(s[1] == '\x8F');
    CHECK(s[2] == '\xBF');
    CHECK(s[3] == '\xBF');
    CHECK(s == u8"\U0010FFFF");
}
