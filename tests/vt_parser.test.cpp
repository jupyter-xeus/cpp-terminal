/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2026 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cpp-terminal/private/vt_parser.hpp"

#include "cpp-terminal/win_adapter.hpp"
#include "doctest/doctest.h"

#include <sstream>

using namespace Term::Private;

namespace
{
constexpr auto& csi = vt::csi;

std::string esc(const char* code) { return std::string("\x1b") + code; }

std::string osc(const char* code) { return std::string("\x1b]") + code + std::string("\x1b\x5c"); }

std::string osc_bell(const char* code) { return std::string("\x1b]") + code + std::string("\x07"); }
}  // namespace

TEST_CASE("Parse. Handles bad CSI sequence")
{
  auto result = parse(csi("3;1") + csi("2J"));

  REQUIRE(result.size() == 2);
  CHECK(result[0].type() == VtSequence::Type::Malformed);
  CHECK(result[1].type() == VtSequence::Type::CSI);
}

TEST_CASE("Parse. Handles split sequences")
{
  std::stringstream       stream;
  std::vector<VtSequence> sequences;

  auto cb = [&sequences](VtSequence seq) { sequences.push_back(std::move(seq)); };

  stream << "\x1b[48;2";
  parse(stream.rdbuf(), cb);

  stream << ";233;171;23m";
  parse(stream.rdbuf(), cb);

  REQUIRE(sequences.size() == 1);
  CHECK(sequences[0].type() == VtSequence::Type::CSI);
  CHECK(sequences[0].content() == "48;2;233;171;23m");
}

TEST_CASE("Parse. Handles split UTF-8")
{
  std::stringstream       stream;
  std::vector<VtSequence> sequences;

  auto cb = [&sequences](VtSequence seq) { sequences.push_back(std::move(seq)); };

  std::string str = "│";

  for(auto c: str)
  {
    stream << c;
    parse(stream.rdbuf(), cb);
  }

  REQUIRE(sequences.size() == 1);
  CHECK(sequences[0].type() == VtSequence::Type::Text);
  CHECK(sequences[0].content() == "│");
}

TEST_CASE("Parse. Ground state handles C0")
{
  auto result = parse("Hi there!\x0d");

  REQUIRE(result.size() == 2);
  CHECK(result[0].type() == VtSequence::Type::Text);
  CHECK(result[1].type() == VtSequence::Type::C0);
}

TEST_CASE("Parse. Ground state handles multibyte UTF8")
{
  auto result = parse("It's 🍺 o'clock");

  REQUIRE(result.size() == 1);
  CHECK(result[0].type() == VtSequence::Type::Text);

  auto& content = result[0].content();
  CHECK(content.at(5) == '\xF0');
  CHECK(content.at(6) == '\x9F');
  CHECK(content.at(7) == '\x8D');
  CHECK(content.at(8) == '\xBA');
}

TEST_CASE("Parse. 24-bit color sequence")
{
  auto result = parse("\x1b[48;2;233;171;23m\x1b[KRumble Bee Yellow\x1b[0m");

  CHECK(result.size() == 4);

  CHECK(result[0].type() == Term::Private::VtSequence::Type::CSI);
  CHECK(result[0].content() == "48;2;233;171;23m");

  CHECK(result[1].type() == Term::Private::VtSequence::Type::CSI);
  CHECK(result[1].content() == "K");

  CHECK(result[2].type() == Term::Private::VtSequence::Type::Text);
  CHECK(result[2].content() == "Rumble Bee Yellow");

  CHECK(result[3].type() == Term::Private::VtSequence::Type::CSI);
  CHECK(result[3].content() == "0m");
}

TEST_CASE("Parse. Test various OSC terminators")
{
  /*
  * Apparently, there are three ways to terminate an OSC sequence.
  *
  * https://vt100.net/emu/dec_ansi_parser
  * Says terminator is 0x9C
  *
  * https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#window-title
  * Says terminator is the sequence 0x1B 0x5C (ESC /), but BEL (0x7) can also be used.
  *
  * https://en.wikipedia.org/wiki/ANSI_escape_code#Operating_System_Command_sequences
  * Says terminator can be BEL (0x07) as well as the standard ST (0x9C or 0x1B 0x5C).
  *
  * Supporting all three should not be a problem. Several references have noted that the
  * 'ESC \' long form is preferred.
  *
  */

  auto result = parse("\x1b]cmd\x1b\x5c");
  REQUIRE(result.size() == 2);
  CHECK(result[0].type() == VtSequence::Type::OSC);
  CHECK(result[1].type() == VtSequence::Type::Escape);

  result = parse("\x1b]cmd\x07");
  REQUIRE(result.size() == 1);
  CHECK(result[0].type() == VtSequence::Type::OSC);

  result = parse("\x1b]cmd\x9c");
  REQUIRE(result.size() == 1);
  CHECK(result[0].type() == VtSequence::Type::OSC);
}

TEST_CASE("Parse. OSC clears malformed sequence before transition")
{
  auto result = parse("\x1b]cmd\x18");
  REQUIRE(result.size() == 1);
  CHECK(result[0].type() == VtSequence::Type::Malformed);
}

TEST_CASE("Parse. OSC malformed if no content")
{
  auto result = parse("\x1b]\x9c");
  REQUIRE(result.size() == 1);
  CHECK(result[0].type() == VtSequence::Type::Malformed);
}

TEST_CASE("Parse. OSC accepts 0x9d as initiating char")
{
  std::string content = "cmd";
  std::string seq("\x9d");
  seq += content + "\x9c";
  auto result = parse(seq);
  REQUIRE(result.size() == 1);
  CHECK(result[0].type() == VtSequence::Type::OSC);
  CHECK(result[0].content() == content);
}

TEST_CASE("VtCommand. Parses intermediates correctly")
{
  VtSequence seq(VtSequence::Type::CSI, csi(" q"));
  VtCommand  cmd(seq);
  REQUIRE(cmd.intermediates().size() == 1);
  CHECK(cmd.intermediates()[0] == 0x20);
}

TEST_CASE("VtCommand. Ignores excess parameters")
{
  // The Scroll Up command expects zero or one parameters.
  VtSequence seq(VtSequence::Type::CSI, csi("1;2;3;4;5;6;7S"));
  VtCommand  cmd(seq);
  CHECK(cmd.type() == VtCommand::Type::ViewportScrollUp);
  REQUIRE(cmd.params().size() == 7);
  CHECK(cmd.params()[0] == 1);
  CHECK(cmd.params()[6] == 7);
}

TEST_CASE("VtCommand. Adds placeholder parameters")
{
  // The Cursor Position command takes two parameters. If the first one is not specified,
  // but the second one is, we need a way to know that.
  VtSequence seq(VtSequence::Type::CSI, csi(";2H"));
  VtCommand  cmd(seq);
  CHECK(cmd.type() == VtCommand::Type::CursorPositionAbsolute);
  REQUIRE(cmd.params().size() == 2);
  CHECK(cmd.params()[0] == 0);
  CHECK(cmd.params()[1] == 2);
}

TEST_CASE("VtCommand. Does not allow negative parameters")
{
  /*
  * Note:
  * This test succeeds not because the parser is checking for a negative number so that it
  * may be rejected (although it does). It succeeds because of the way the parser handles the
  * negative sign (0x2d). Once it encounters it, it stops parsing parameters because the
  * negative sign is in the intermediate character range.
  */
  VtSequence seq(VtSequence::Type::CSI, csi("1;-1H"));
  VtCommand  cmd(seq);
  CHECK(cmd.type() == VtCommand::Type::Invalid);
}

TEST_CASE("VtCommand. Does not allow parameters values larger than the maximum short value")
{
  VtSequence seq_good(VtSequence::Type::CSI, csi("32767A"));
  VtCommand  cmd_good(seq_good);
  CHECK(cmd_good.type() == VtCommand::Type::CursorUp);
  REQUIRE(cmd_good.params().size() == 1);
  CHECK(cmd_good.params()[0] == 32767);

  VtSequence seq_bad(VtSequence::Type::CSI, csi("32768A"));
  VtCommand  cmd_bad(seq_bad);
  CHECK(cmd_bad.type() == VtCommand::Type::Invalid);
}

TEST_CASE("VtCommand. Handles invalid final character")
{
  VtSequence seq(VtSequence::Type::CSI, csi("1"));
  VtCommand  cmd(seq);
  CHECK(cmd.type() == VtCommand::Type::Invalid);
}

TEST_CASE("Parse. Simple cursor positioning sequences")
{
  auto result = parse(esc("M") + esc("7") + esc("8"));

  REQUIRE(result.size() == 3);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorUp, "M should be CursorUp"); break;
      case 1: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorSave, "7 should be CursorSave"); break;
      case 2: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorRestore, "8 should be CursorRestore"); break;
    }
  }
}

TEST_CASE("Parse. Cursor positioning sequences")
{
  auto result = parse(csi("A") + csi("1B") + csi("2C") + csi("20D") + csi("200E") + csi("2000F") + csi("20000G") + csi("0d") + csi("H") + csi("1;1f") + csi("s") + csi("u"));

  REQUIRE(result.size() == 12);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorUp, "A should be CursorUp");
        REQUIRE_MESSAGE(cmd.params().size() == 0, "A should have no parameters");
        break;
      case 1:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorDown, "B should be CursorDown");
        REQUIRE_MESSAGE(cmd.params().size() == 1, "B should have one parameter");
        CHECK_MESSAGE(cmd.params()[0] == 1, "B should have first parameter = 1");
        break;
      case 2:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorRight, "C should be CursorRight");
        REQUIRE_MESSAGE(cmd.params().size() == 1, "C should have one parameter");
        CHECK_MESSAGE(cmd.params()[0] == 2, "C should have first parameter = 2");
        break;
      case 3:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorLeft, "D should be CursorLeft");
        REQUIRE_MESSAGE(cmd.params().size() == 1, "D should have one parameter");
        CHECK_MESSAGE(cmd.params()[0] == 20, "D should have first parameter = 20");
        break;
      case 4:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorNextLine, "E should be CursorNextLine");
        REQUIRE_MESSAGE(cmd.params().size() == 1, "E should have one parameter");
        CHECK_MESSAGE(cmd.params()[0] == 200, "E should have first parameter = 200");
        break;
      case 5:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorPreviousLine, "F should be CursorPreviousLine");
        REQUIRE_MESSAGE(cmd.params().size() == 1, "F should have one parameter");
        CHECK_MESSAGE(cmd.params()[0] == 2000, "F should have first parameter = 2000");
        break;
      case 6:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorHorizontalAbsolute, "G should be CursorHorizontalAbsolute");
        REQUIRE_MESSAGE(cmd.params().size() == 1, "G should have one parameter");
        CHECK_MESSAGE(cmd.params()[0] == 20000, "G should have first parameter = 20000");
        break;
      case 7:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorVerticalAbsolute, "d should be CursorVerticalAbsolute");
        REQUIRE_MESSAGE(cmd.params().size() == 1, "d should have one parameters");
        CHECK_MESSAGE(cmd.params()[0] == 0, "d should have first parameter = 0");
        break;
      case 8:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorPositionAbsolute, "H should be CursorPositionAbsolute");
        REQUIRE_MESSAGE(cmd.params().size() == 0, "H should have no parameters");
        break;
      case 9:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorPositionAbsolute, "f should be CursorPositionAbsolute");
        REQUIRE_MESSAGE(cmd.params().size() == 2, "f should have two parameters");
        CHECK_MESSAGE(cmd.params()[0] == 1, "f should have first parameter = 1");
        CHECK_MESSAGE(cmd.params()[1] == 1, "f should have second parameter = 1");
        break;
      case 10:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorSave, "s should be CursorSave");
        REQUIRE_MESSAGE(cmd.params().size() == 0, "s should have no parameters");
        break;
      case 11:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorRestore, "u should be CursorRestore");
        REQUIRE_MESSAGE(cmd.params().size() == 0, "u should have no parameters");
        break;
    }
  }
}

TEST_CASE("Parse. Cursor visibility sequences")
{
  auto result = parse(csi("?12h") + csi("?12l") + csi("?25h") + csi("?25l"));

  REQUIRE(result.size() == 4);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorBlink, "12h should be CursorBlink"); break;
      case 1: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorNoBlink, "12l should be CursorNoBlink"); break;
      case 2: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorShow, "25h should be CursorShow"); break;
      case 3: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorHide, "25l should be CursorHide"); break;
    }
  }
}

TEST_CASE("Parse. Cursor shape sequences")
{
  // https://vt100.net/docs/vt510-rm/DECSCUSR.html
  // https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#cursor-shape
  auto result = parse("\x1b[ q\x1b[0 q\x1b[1 q\x1b[2 q\x1b[3 q\x1b[4 q\x1b[5 q\x1b[6 q");

  REQUIRE(result.size() == 8);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0:
      case 1: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorDefaultShape, "No param or zero should be CursorDefaultShape"); break;
      case 2: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorBlinkingBlock, "Param 1 should be CursorBlinkingBlock"); break;
      case 3: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorSteadyBlock, "Param 2 should be CursorSteadyBlock"); break;
      case 4: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorBlinkingUnderline, "Param 3 should be CursorBlinkingUnderline"); break;
      case 5: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorSteadyUnderline, "Param 4 should be CursorSteadyUnderline"); break;
      case 6: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorBlinkingBar, "Param 5 should be CursorBlinkingBar"); break;
      case 7: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CursorSteadyBar, "Param 6 should be CursorSteadyBar"); break;
    }
  }
}

TEST_CASE("Parse. Viewport positioning sequences")
{
  auto result = parse(csi("1S") + csi("2T"));

  REQUIRE(result.size() == 2);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::ViewportScrollUp, "S should be ViewportScrollUp");
        REQUIRE_MESSAGE(cmd.params().size() == 1, "S should have one parameter");
        CHECK_MESSAGE(cmd.params()[0] == 1, "S should have first parameter = 1");
        break;
      case 1:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::ViewportScrollDown, "T should be ViewportScrollDown");
        REQUIRE_MESSAGE(cmd.params().size() == 1, "T should have one parameter");
        CHECK_MESSAGE(cmd.params()[0] == 2, "T should have first parameter = 2");
        break;
    }
  }
}

TEST_CASE("Parse. Text modification sequences")
{
  auto result = parse(csi("@") + csi("P") + csi("X") + csi("L") + csi("M") + csi("J") + csi("K") + csi("3J") + csi("4J") + csi("3K"));

  REQUIRE(result.size() == 10);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TextInsertCharacter, "@ should be TextInsertCharacter"); break;
      case 1: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TextDeleteCharacter, "P should be TextDeleteCharacter"); break;
      case 2: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TextEraseCharacter, "X should be TextEraseCharacter"); break;
      case 3: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TextInsertLine, "L should be TextInsertLine"); break;
      case 4: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TextDeleteLine, "M should be TextDeleteLine"); break;
      case 5: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TextEraseInDisplay, "J should be TextEraseInDisplay"); break;
      case 6: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TextEraseInLine, "K should be TextEraseInLine"); break;
      case 7: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TextEraseInDisplay, "3J should be TextEraseInDisplay (3 was added in Linux 3.0)"); break;
      case 8: CHECK_MESSAGE(cmd.type() == VtCommand::Type::Invalid, "The only valid parameter values for J are 0, 1, 2 and 3"); break;
      case 9: CHECK_MESSAGE(cmd.type() == VtCommand::Type::Invalid, "The only valid parameter values for K are 0, 1 and 2"); break;
    }
  }
}

TEST_CASE("Parse. Screen color sequences")
{
  auto result = parse(osc_bell("4;3;rgb:0/128/255") + osc_bell("4;16;rgb:64/128/255") + osc_bell("4;16;rgb:256/32/32"));

  REQUIRE(result.size() == 3);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::ScreenColorPaletteSet, "'4' should be ScreenColorPaletteSet");
        REQUIRE(cmd.params().size() == 4);
        CHECK_MESSAGE(cmd.params()[0] == 3, "Palette index should be 3");
        CHECK_MESSAGE(cmd.params()[1] == 0, "R value should be 0");
        CHECK_MESSAGE(cmd.params()[2] == 128, "G value should be 128");
        CHECK_MESSAGE(cmd.params()[3] == 255, "B value should be 255");
        break;
      case 1: CHECK_MESSAGE(cmd.type() == VtCommand::Type::Invalid, "Out of bounds color palette index value. Valid range is 0-15."); break;
      case 2: CHECK_MESSAGE(cmd.type() == VtCommand::Type::Invalid, "Out of bounds RGB value. Valid range is 0-255."); break;
    }
  }
}

TEST_CASE("Parse. Mode change sequences")
{
  auto result = parse(esc("=") + esc(">") + csi("?1h") + csi("?1l"));

  REQUIRE(result.size() == 4);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0: CHECK_MESSAGE(cmd.type() == VtCommand::Type::ModeKeypadApplication, "'ESC =' should be ModeKeypadApplication"); break;
      case 1: CHECK_MESSAGE(cmd.type() == VtCommand::Type::ModeKeypadNumeric, "'ESC >' should be ModeKeypadNumeric"); break;
      case 2: CHECK_MESSAGE(cmd.type() == VtCommand::Type::ModeCursorKeysApplication, "?1h should be ModeCursorKeysApplication"); break;
      case 3: CHECK_MESSAGE(cmd.type() == VtCommand::Type::ModeCursorKeysNumeric, "?1l should be ModeCursorKeysNumeric"); break;
    }
  }
}

TEST_CASE("Parse. Query state sequences")
{
  auto result = parse(csi("6n") + csi("0c"));

  REQUIRE(result.size() == 2);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0: CHECK_MESSAGE(cmd.type() == VtCommand::Type::QueryCursorPosition, "6n should be QueryCursorPosition"); break;
      case 1: CHECK_MESSAGE(cmd.type() == VtCommand::Type::QueryDeviceAttributes, "0c should be QueryDeviceAttributes"); break;
    }
  }
}

TEST_CASE("Parse. Tab setting sequences")
{
  auto result = parse(esc("H") + csi("I") + csi("Z") + csi("0g") + csi("3g"));

  REQUIRE(result.size() == 5);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TabCursorSet, "'ESC H' should be TabCursorSet"); break;
      case 1: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TabCursorForward, "I should be TabCursorForward"); break;
      case 2: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TabCursorBackward, "Z should be TabCursorBackward"); break;
      case 3: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TabCursorClearCurrent, "0g should be TabCursorClearCurrent"); break;
      case 4: CHECK_MESSAGE(cmd.type() == VtCommand::Type::TabClearAll, "3g should be TabClearAll"); break;
    }
  }
}

TEST_CASE("Parse. Character set designation sequences")
{
  auto result = parse(esc("(0") + esc("(B"));

  REQUIRE(result.size() == 2);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CharsetDecLineDrawing, "'(0' should be CharsetDecLineDrawing"); break;
      case 1: CHECK_MESSAGE(cmd.type() == VtCommand::Type::CharsetUsAscii, "'(B' should be CharsetUsAscii"); break;
    }
  }
}

TEST_CASE("Parse. Scrolling margin sequences")
{
  auto result = parse(csi("r"));

  REQUIRE(result.size() == 1);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0: CHECK_MESSAGE(cmd.type() == VtCommand::Type::ScrollingRegionSet, "'r' should be ScrollingRegionSet"); break;
    }
  }
}

TEST_CASE("Parse. Window title sequences")
{
  auto result = parse(osc_bell("0;test 0") + osc_bell("2;test 2"));

  REQUIRE(result.size() == 2);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    std::string title;
    for(const auto c: cmd.intermediates()) { title += c; }

    switch(i)
    {
      case 0:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::WindowTitleSet, "'0' should be WindowTitleSet");
        CHECK_MESSAGE(title == "test 0", "'0' title should be 'test 0'");
        break;
      case 1:
        CHECK_MESSAGE(cmd.type() == VtCommand::Type::WindowTitleSet, "'2' should be WindowTitleSet");
        CHECK_MESSAGE(title == "test 2", "'2' title should be 'test 0'");
        break;
    }
  }
}

TEST_CASE("Parse. Window title with split UTF-8")
{
  std::stringstream       stream;
  std::vector<VtSequence> sequences;

  auto cb = [&sequences](VtSequence seq) { sequences.push_back(std::move(seq)); };

  // "window" in Persian
  std::string title_orig = "پنجره";

  std::string str = "\x1b]0;";
  str += title_orig;
  str += "\x07";

  for(auto c: str)
  {
    stream << c;
    parse(stream.rdbuf(), cb);
  }

  REQUIRE(sequences.size() == 1);

  VtCommand cmd(sequences[0]);

  std::string title_rcvd;
  for(const auto c: cmd.intermediates()) { title_rcvd += c; }

  CHECK(cmd.type() == VtCommand::Type::WindowTitleSet);
  CHECK(title_rcvd == title_orig);
}

TEST_CASE("Parse. Screen buffer sequences")
{
  auto result = parse(csi("?1049h") + csi("?1049l"));

  REQUIRE(result.size() == 2);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0: CHECK_MESSAGE(cmd.type() == VtCommand::Type::ScreenBufferAlternate, "'1049h' should be ScreenBufferAlternate"); break;
      case 1: CHECK_MESSAGE(cmd.type() == VtCommand::Type::ScreenBufferMain, "'1049l' should be ScreenBufferMain"); break;
    }
  }
}

TEST_CASE("Parse. Window width sequences")
{
  auto result = parse(csi("?3h") + csi("?3l"));

  REQUIRE(result.size() == 2);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0: CHECK_MESSAGE(cmd.type() == VtCommand::Type::WindowWidth132, "'3h' should be WindowWidth132"); break;
      case 1: CHECK_MESSAGE(cmd.type() == VtCommand::Type::WindowWidth80, "'3l' should be WindowWidth80"); break;
    }
  }
}

TEST_CASE("Parse. Soft reset sequences")
{
  auto result = parse(csi("!p"));

  REQUIRE(result.size() == 1);

  for(int i = 0; i < result.size(); ++i)
  {
    VtCommand cmd(result[i]);

    switch(i)
    {
      case 0: CHECK_MESSAGE(cmd.type() == VtCommand::Type::SoftReset, "'!p' should be SoftReset"); break;
    }
  }
}

TEST_CASE("Parse. SGR extended color sequences")
{
  VtCommand cmd;

  auto result = parse(csi("38;2;0;128;255m"));
  cmd         = VtCommand(result[0]);
  CHECK(cmd.type() == VtCommand::Type::Sgr);
  REQUIRE(cmd.params().size() == 5);
  CHECK(cmd.params()[0] == 38);
  CHECK(cmd.params()[1] == 2);
  CHECK(cmd.params()[2] == 0);
  CHECK(cmd.params()[3] == 128);
  CHECK(cmd.params()[4] == 255);

  result = parse(csi("48;2;0;128;255m"));
  cmd    = VtCommand(result[0]);
  CHECK(cmd.type() == VtCommand::Type::Sgr);
  REQUIRE(cmd.params().size() == 5);
  CHECK(cmd.params()[0] == 48);
  CHECK(cmd.params()[1] == 2);
  CHECK(cmd.params()[2] == 0);
  CHECK(cmd.params()[3] == 128);
  CHECK(cmd.params()[4] == 255);

  // Default RGB values
  result = parse(csi("48;2;;;m"));
  cmd    = VtCommand(result[0]);
  CHECK(cmd.type() == VtCommand::Type::Sgr);
  REQUIRE(cmd.params().size() == 5);
  CHECK(cmd.params()[0] == 48);
  CHECK(cmd.params()[1] == 2);
  CHECK(cmd.params()[2] == 0);
  CHECK(cmd.params()[3] == 0);
  CHECK(cmd.params()[4] == 0);

  result = parse(csi("38;5;255m"));
  cmd    = VtCommand(result[0]);
  CHECK(cmd.type() == VtCommand::Type::Sgr);
  REQUIRE(cmd.params().size() == 3);
  CHECK(cmd.params()[0] == 38);
  CHECK(cmd.params()[1] == 5);
  CHECK(cmd.params()[2] == 255);

  result = parse(csi("48;5;255m"));
  cmd    = VtCommand(result[0]);
  CHECK(cmd.type() == VtCommand::Type::Sgr);
  REQUIRE(cmd.params().size() == 3);
  CHECK(cmd.params()[0] == 48);
  CHECK(cmd.params()[1] == 5);
  CHECK(cmd.params()[2] == 255);

  // Default index
  result = parse(csi("48;5;m"));
  cmd    = VtCommand(result[0]);
  CHECK(cmd.type() == VtCommand::Type::Sgr);
  REQUIRE(cmd.params().size() == 3);
  CHECK(cmd.params()[0] == 48);
  CHECK(cmd.params()[1] == 5);
  CHECK(cmd.params()[2] == 0);

  result = parse(csi("48;2;256;128;255m"));
  cmd    = VtCommand(result[0]);
  CHECK_MESSAGE(cmd.type() == VtCommand::Type::Invalid, "Should be invalid - RGB value is out of range");

  result = parse(csi("48;2;256;128m"));
  cmd    = VtCommand(result[0]);
  CHECK_MESSAGE(cmd.type() == VtCommand::Type::Invalid, "Should be invalid - RGB value is missing");

  result = parse(csi("38;5;256m"));
  cmd    = VtCommand(result[0]);
  CHECK_MESSAGE(cmd.type() == VtCommand::Type::Invalid, "Should be invalid - index value is out of range");

  result = parse(csi("38;5m"));
  cmd    = VtCommand(result[0]);
  CHECK_MESSAGE(cmd.type() == VtCommand::Type::Invalid, "Should be invalid - index value is missing");
}

namespace
{
vt::state default_state()
{
  vt::state state;

  state.screen_buffer.height = 9001;
  state.screen_buffer.width  = 120;

  state.cursor_position = Term::Cursor({Term::Column(1), Term::Row(1)});

  state.foreground_color = 37;
  state.background_color = 40;

  state.window.top    = 1;
  state.window.right  = 120;
  state.window.bottom = 30;
  state.window.left   = 1;

  return state;
}

struct TestApi : VtApi
{
  TestApi()
  {
    handle_open = [](VtApi::TerminalHandleType, std::function<void(vt::Handle::raw_handle_type)> closer) { return vt::Handle((vt::Handle::raw_handle_type)0x1234, closer); };

    handle_close = [](vt::Handle::raw_handle_type) {};

    get_state = [](const vt::Handle&) { return default_state(); };

    scroll = [](const vt::Handle&, const vt::rectangle& /*scroll*/
                ,
                const vt::rectangle& /*clip*/
                ,
                const vt::point& /*destination*/
                ,
                const vt::fill& /*fill*/) {};

    fill = [](const vt::Handle&, const vt::fill& /*fill*/, const vt::point& /*at*/, std::size_t /*len*/) {};

    set_cursor_position = [](const vt::Handle&, const Term::Cursor& /*cursor*/) {};

    get_cursor_appearance = [](const vt::Handle&)
    {
      vt::cursor_appearance ca;
      ca.fill_percent = 25;
      ca.visible      = true;
      return ca;
    };

    set_cursor_appearance = [](const vt::Handle&, const vt::cursor_appearance& /*ca*/) {};

    get_screen_buffer_defaults = [](const vt::Handle&)
    {
      vt::screen_buffer_defaults defaults;
      defaults.cursor_fill_percent = 25;
      defaults.foreground_color    = 37;
      defaults.background_color    = 40;
      return defaults;
    };

    set_text_attributes      = [](const vt::Handle&, const vt::text_attributes& /*attribs*/) {};
    write_to_input           = [](const vt::Handle&, const std::string& /*str*/) {};
    set_palette_color        = [](const vt::Handle&, vt::tiny_type /*index*/, const vt::rgb& /*rgb*/) {};
    write_to_output          = [](const vt::Handle&, const std::string& /*str*/) {};
    set_title                = [](const vt::Handle&, const std::string& /*str*/) {};
    set_active_screen_buffer = [](const vt::Handle&) {};
    set_window_width         = [](const vt::Handle&, vt::dim_type /*width*/) {};
  }
};
}  // namespace

TEST_CASE("VtEmulator. Cursor positioning")
{
  TestApi api;

  auto state = default_state();

  api.get_state = [&state](const vt::Handle&) { return state; };

  api.set_cursor_position = [&state](const vt::Handle&, const Term::Cursor& cursor) { state.cursor_position = cursor; };

  VtEmulator emulator(api);

  // Move 4 spaces forward
  auto         result          = parse(csi("C") + csi("1C") + csi("2C"));
  Term::Cursor expected_cursor = Term::Cursor({Term::Column(5), Term::Row(1)});

  for(auto& seq: result) { emulator.process(seq); }

  REQUIRE(state.cursor_position == expected_cursor);

  // Move 4 spaces down
  result          = parse(csi("B") + csi("1B") + csi("2B"));
  expected_cursor = Term::Cursor({Term::Column(5), Term::Row(5)});

  for(auto& seq: result) { emulator.process(seq); }

  REQUIRE(state.cursor_position == expected_cursor);

  // Save the current cursor position
  result = parse(esc("7"));
  emulator.process(result[0]);
  Term::Cursor saved_cursor_pos = expected_cursor;

  // Move 4 spaces backward
  result          = parse(csi("D") + csi("1D") + csi("2D"));
  expected_cursor = Term::Cursor({Term::Column(1), Term::Row(5)});

  for(auto& seq: result) { emulator.process(seq); }

  REQUIRE(state.cursor_position == expected_cursor);

  // Move 4 spaces up
  result          = parse(csi("A") + csi("1A") + csi("2A"));
  expected_cursor = Term::Cursor({Term::Column(1), Term::Row(1)});

  for(auto& seq: result) { emulator.process(seq); }

  REQUIRE(state.cursor_position == expected_cursor);  // 5, 5

  // Restore the saved cursor position
  result = parse(esc("8"));
  emulator.process(result[0]);

  REQUIRE(state.cursor_position == saved_cursor_pos);

  // Move down one line
  result          = parse(csi("E"));
  expected_cursor = Term::Cursor({Term::Column(1), Term::Row(6)});

  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  //Absolute position
  result          = parse(csi("5;10H"));
  expected_cursor = Term::Cursor({Term::Column(10), Term::Row(5)});

  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Move up one line
  result          = parse(csi("F"));
  expected_cursor = Term::Cursor({Term::Column(1), Term::Row(4)});

  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Horizontal absolute
  result          = parse(csi("7G"));
  expected_cursor = Term::Cursor({Term::Column(7), Term::Row(4)});

  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Vertical absolute
  result          = parse(csi("7d"));
  expected_cursor = Term::Cursor({Term::Column(7), Term::Row(7)});

  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Attempt to move outside buffer range
  auto code = std::to_string(state.screen_buffer.width + 1) + "G";
  result    = parse(csi(code.c_str()));
  emulator.process(result[0]);
  REQUIRE_MESSAGE(state.cursor_position == expected_cursor, "Cursor should not have moved");
}

TEST_CASE("VtEmulator. Text formatting")
{
  TestApi api;

  auto state = default_state();

  api.get_state = [&state](const vt::Handle&) { return state; };

  api.set_text_attributes = [&state](const vt::Handle&, const vt::text_attributes& attribs)
  {
    state.foreground_color = attribs.foreground_color;
    state.background_color = attribs.background_color;
  };

  VtEmulator emulator(api);

  // Select colors with original codes fg: red, bg: green
  auto result = parse(csi("31;42m"));
  emulator.process(result[0]);
  CHECK(state.foreground_color == 31);
  CHECK(state.background_color == 42);

  // Select colors from 256 color table fg: green, bg: yellow
  result = parse(csi("38;5;34;48;5;142m"));
  emulator.process(result[0]);
  CHECK(state.foreground_color == 32);
  CHECK(state.background_color == 43);

  // Select colors using RGB values fg: yellow, bg: blue
  result = parse(csi("38;2;127;127;0;48;2;0;0;127m"));
  emulator.process(result[0]);
  CHECK(state.foreground_color == 33);
  CHECK(state.background_color == 44);

  // Add bold (bright foreground) and underline (bright background)
  result = parse(csi("1;4m"));
  emulator.process(result[0]);
  CHECK(state.foreground_color == 93);
  CHECK(state.background_color == 104);

  // Reverse foreground and background
  result = parse(csi("7m"));
  emulator.process(result[0]);
  CHECK(state.foreground_color == 94);
  CHECK(state.background_color == 103);
}

TEST_CASE("VtEmulator. Tabs")
{
  TestApi api;

  auto state = default_state();

  api.get_state = [&state](const vt::Handle&) { return state; };

  api.set_cursor_position = [&state](const vt::Handle&, const Term::Cursor& cursor) { state.cursor_position = cursor; };

  VtEmulator emulator(api);

  // There are implicit tabs at the first and last column of the window
  // even if no tabs have been explicitly set.
  auto         result          = parse(csi("I"));
  Term::Cursor expected_cursor = Term::Cursor({Term::Column(state.screen_buffer.width), Term::Row(1)});
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // If cursor is in the last column, tab moves to beginning of next line
  result          = parse(csi("I"));
  expected_cursor = Term::Cursor({Term::Column(1), Term::Row(2)});
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Set tabs at 40 and 80 and tab back
  result          = parse(csi("40G") + esc("H") + csi("80G") + esc("H") + csi("Z"));
  expected_cursor = Term::Cursor({Term::Column(40), Term::Row(2)});

  for(auto& seq: result) { emulator.process(seq); }

  REQUIRE(state.cursor_position == expected_cursor);

  // Another tab back
  result          = parse(csi("Z"));
  expected_cursor = Term::Cursor({Term::Column(1), Term::Row(2)});
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Another tab back - unlike forward, which proceeds to the next line if at the end,
  // back does not move if it's already at the far left.
  result = parse(csi("Z"));
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Tab forward (1 -> 40)
  result          = parse(csi("I"));
  expected_cursor = Term::Cursor({Term::Column(40), Term::Row(2)});
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Tab forward (40 -> 80)
  result          = parse(csi("1I"));
  expected_cursor = Term::Cursor({Term::Column(80), Term::Row(2)});
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Tab forward (80 -> far right)
  result          = parse(csi("1I"));
  expected_cursor = Term::Cursor({Term::Column(state.screen_buffer.width), Term::Row(2)});
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Tab forward (far right -> 1, next row)
  result          = parse(csi("I"));
  expected_cursor = Term::Cursor({Term::Column(1), Term::Row(3)});
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // 3 tabs forward (1 -> far right)
  result          = parse(csi("3I"));
  expected_cursor = Term::Cursor({Term::Column(state.screen_buffer.width), Term::Row(3)});
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // 2 tabs backward (far right -> 40)
  result          = parse(csi("2Z"));
  expected_cursor = Term::Cursor({Term::Column(40), Term::Row(3)});
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Clear tab at current position and tab back (40 -> 1)
  result          = parse(csi("0g") + csi("Z"));
  expected_cursor = Term::Cursor({Term::Column(1), Term::Row(3)});

  for(auto& seq: result) { emulator.process(seq); }

  REQUIRE(state.cursor_position == expected_cursor);

  // Tab forward (1 -> 80)
  result          = parse(csi("I"));
  expected_cursor = Term::Cursor({Term::Column(80), Term::Row(3)});
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);

  // Tab back and clear all tabs (80 -> 1)
  result          = parse(csi("Z") + csi("3g"));
  expected_cursor = Term::Cursor({Term::Column(1), Term::Row(3)});

  for(auto& seq: result) { emulator.process(seq); }

  REQUIRE(state.cursor_position == expected_cursor);

  // Tab forward (1 -> far right)
  result          = parse(csi("I"));
  expected_cursor = Term::Cursor({Term::Column(state.screen_buffer.width), Term::Row(3)});
  emulator.process(result[0]);
  REQUIRE(state.cursor_position == expected_cursor);
}

TEST_CASE("VtEmulator. Set scrolling region")
{
  TestApi api;

  auto state = default_state();

  api.get_state = [&state](const vt::Handle&) { return state; };

  api.set_cursor_position = [&state](const vt::Handle&, const Term::Cursor& cursor) { state.cursor_position = cursor; };

  vt::rectangle clip_rectangle;
  api.scroll = [&clip_rectangle](const vt::Handle&, const vt::rectangle&, const vt::rectangle& clip, const vt::point&, const vt::fill&) { clip_rectangle = clip; };

  VtEmulator emulator(api);

  // Check default values. Scroll so we can get a look at the resulting clip rectangle.
  auto result = parse(csi("S"));
  emulator.process(result[0]);

  CHECK(clip_rectangle.top == state.window.top);
  CHECK(clip_rectangle.bottom == state.window.bottom);

  // Move cursor to somewhere other than top-left, set margin to t: 5, b: default
  result = parse(csi("7;7H") + csi("5;r") + csi("S"));
  for(auto& seq: result) { emulator.process(seq); }

  CHECK(clip_rectangle.top == 5);
  CHECK(clip_rectangle.bottom == state.window.bottom);
  CHECK(state.cursor_position == Term::Cursor({Term::Column(state.window.left), Term::Row(state.window.top)}));
}

TEST_CASE("VtEmulator. Character set")
{
  TestApi api;

  std::string output;
  api.write_to_output = [&output](const vt::Handle&, const std::string& str) { output += str; };

  VtEmulator emulator(api);

  auto result = parse(esc("(0")    // Select DEC Special Graphics character set
                      + "k"        // Upper-right corner (U+2510, represented as UTF-8 sequence 0xE2 0x94 0x90)
                      + esc("(B")  // Select US ASCII character set
                      + "k"        // lower case letter 'k' (U+006b)
  );

  for(auto& seq: result) { emulator.process(seq); }

  REQUIRE(output.size() == 4);
  CHECK(output[0] == '\xe2');
  CHECK(output[1] == '\x94');
  CHECK(output[2] == '\x90');
  CHECK(output[3] == 'k');
}

TEST_CASE("VtEmulator. Set title")
{
  TestApi api;

  std::string actual_title;
  api.set_title = [&actual_title](const vt::Handle&, const std::string& str) { actual_title += str; };

  VtEmulator emulator(api);

  std::string expected_title = "first method";
  std::string cmd            = "0;" + expected_title;
  auto        result         = parse(osc_bell(cmd.c_str()));
  emulator.process(result[0]);
  CHECK(actual_title == expected_title);

  actual_title.clear();
  expected_title = "I'm glad there are two ways to do this! 🍺";
  cmd            = "2;" + expected_title;
  result         = parse(osc_bell(cmd.c_str()));
  emulator.process(result[0]);
  CHECK(actual_title == expected_title);
}

TEST_CASE("VtEmulator. Soft reset")
{
  TestApi api;

  auto state = default_state();

  api.get_state = [&state](const vt::Handle&) { return state; };

  vt::cursor_appearance ca;
  api.set_cursor_appearance = [&ca](const vt::Handle&, const vt::cursor_appearance& ca_set) { ca = ca_set; };

  api.set_cursor_position = [&state](const vt::Handle&, const Term::Cursor& cursor) { state.cursor_position = cursor; };

  std::string output;
  api.write_to_output = [&output](const vt::Handle&, const std::string& str) { output += str; };

  vt::rectangle clip_rectangle;
  api.scroll = [&clip_rectangle](const vt::Handle&, const vt::rectangle&, const vt::rectangle& clip, const vt::point&, const vt::fill&) { clip_rectangle = clip; };

  api.set_text_attributes = [&state](const vt::Handle&, const vt::text_attributes& attribs)
  {
    state.foreground_color = attribs.foreground_color;
    state.background_color = attribs.background_color;
  };

  VtEmulator emulator(api);

  auto original_state = state;

  auto result = parse(csi("10;20r") + csi("S")                // Set margins to top = 10, bottom = 20, then scroll.
                      + esc("(0") + "k"                       // select DEC special graphics character set and get some sample output
                      + csi("32;44;1;4;7m")                   // foreground green, background blue, bold, underline and reverse foreground/background
                      + csi("2;3H") + esc("7") + csi("3;4H")  // Save cursor position at x = 3, y = 2, then move to 4,3
                      + csi("?25l")                           // hide cursor
  );

  for(auto& seq: result) { emulator.process(seq); }

  CHECK(state.cursor_position.column() == 4);
  CHECK(state.cursor_position.row() == 3);
  CHECK(ca.visible == false);
  CHECK(clip_rectangle.top == 10);
  CHECK(clip_rectangle.bottom == 20);
  REQUIRE(output.size() == 3);
  CHECK(output[0] == '\xe2');
  CHECK(output[1] == '\x94');
  CHECK(output[2] == '\x90');
  CHECK_MESSAGE(state.foreground_color == 94, "Foreground color should be bright version of the background color blue (reverse, bold)");
  CHECK_MESSAGE(state.background_color == 102, "Background color should be bright version of the foreground color green (reverse, underline)");
  output.clear();

  // Verify cursor saved at x = 3, y = 2
  result = parse(esc("8"));
  for(auto& seq: result) { emulator.process(seq); }

  CHECK(state.cursor_position.column() == 3);
  CHECK(state.cursor_position.row() == 2);

  // Soft reset
  result = parse(csi("!p") + "k" + csi("S"));
  for(auto& seq: result) { emulator.process(seq); }

  CHECK(ca.visible == true);  // Cursor should be visible
  CHECK(clip_rectangle.top == state.window.top);
  CHECK(clip_rectangle.bottom == state.window.bottom);
  CHECK(output == "k");
  CHECK(state.foreground_color == original_state.foreground_color);
  CHECK(state.background_color == original_state.background_color);

  // As a consequence of resetting margins, the cursor was moved to top left.
  // In order to show the saved cursor is now top left, the current cursor
  // position will changed.
  result = parse(csi("10;15H"));
  for(auto& seq: result) { emulator.process(seq); }

  CHECK(state.cursor_position.column() == 15);
  CHECK(state.cursor_position.row() == 10);

  // Restore saved cursor
  result = parse(esc("8"));
  for(auto& seq: result) { emulator.process(seq); }

  CHECK(state.cursor_position.column() == 1);
  CHECK(state.cursor_position.row() == 1);
}

TEST_CASE("VtEmulator. Select screen buffer")
{
  TestApi            api;
  const std::int64_t primary_handle = 0x100;
  const std::int64_t alt_handle     = 0x200;

  api.handle_open = [&](VtApi::TerminalHandleType type, std::function<void(vt::Handle::raw_handle_type)> closer)
  {
    switch(type)
    {
      case VtApi::TerminalHandleType::OutPrimary: return vt::Handle((vt::Handle::raw_handle_type)primary_handle, closer);
      case VtApi::TerminalHandleType::OutAlternate: return vt::Handle((vt::Handle::raw_handle_type)alt_handle, closer);
      default: return vt::Handle((vt::Handle::raw_handle_type)0x01, closer);
    }
  };

  std::int64_t active_handle_last_set{0};
  api.set_active_screen_buffer = [&active_handle_last_set](const vt::Handle& handle) { active_handle_last_set = (std::int64_t)(vt::Handle::raw_handle_type)handle; };

  std::int64_t active_handle_use_verified{0};
  api.set_text_attributes = [&active_handle_use_verified](const vt::Handle& handle, const vt::text_attributes&) { active_handle_use_verified = (std::int64_t)(vt::Handle::raw_handle_type)handle; };

  {
    VtEmulator emulator(api);

    // Be aware that set_text_attributes is only called if a value changes. The sequences
    // 7m (reverse fg/bg colors) & 27m (unreverse fg/bg colors) are issued throughout this
    // test to force set_text_attributes to be called so that active_handle_use_verified is set.

    // Initial buffer is the primary. 
    auto result = parse(csi("7m"));
    emulator.process(result[0]);
    CHECK(active_handle_use_verified == primary_handle);

    // Switch to alternate. 
    result = parse(csi("?1049h") + csi("7m"));
    for(auto& seq: result) { emulator.process(seq); }

    CHECK(active_handle_use_verified == alt_handle);

    // Switch back to primary
    result = parse(csi("?1049l") + csi("27m"));
    for(auto& seq: result) { emulator.process(seq); }

    CHECK(active_handle_use_verified == primary_handle);

    // Switch back to alternate
    result = parse(csi("?1049h") + csi("27m"));
    for(auto& seq: result) { emulator.process(seq); }

    CHECK(active_handle_last_set == alt_handle);
    CHECK(active_handle_use_verified == alt_handle);
  }

  // Verify the primary screen buffer is restored on destruction of the emulator
  CHECK(active_handle_use_verified == alt_handle);
  CHECK(active_handle_last_set == primary_handle);
}

TEST_CASE("WinConsoleAdapter. Maintains history")
{
  class TestWinConsoleAdapter : public Term::WinConsoleAdapter
  {
  public:
    TestWinConsoleAdapter(std::ostream& stream) : WinConsoleAdapter(stream, 20, 5) {}

    int sync() override
    {
      while(sgetc() != std::streambuf::traits_type::eof())
      {
        m_collected += static_cast<char>(sgetc());
        snextc();
      }

      return 0;
    }

    void rewind(int count)
    {
      for(int i = 0; i < count; ++i) { sungetc(); }
    }

    std::string m_collected;
  };

  std::stringstream     ss;
  TestWinConsoleAdapter buf(ss);

  // Add enough characters to fill buffer, but not overflow.
  const char* fill_buf = "000000000000000abcde";
  ss << fill_buf;

  CHECK(buf.m_collected.empty());

  // Trigger overflow.
  ss << "+";

  CHECK(buf.m_collected == fill_buf);

  // Rewind so all the characters that should be in history can be read again.
  buf.m_collected.clear();
  buf.rewind(5);
  ss << std::flush;

  CHECK(buf.m_collected == "abcde+");
}

TEST_CASE("VtWriter. Handle split sequence")
{
  class TestVtWriter : public VtWriter
  {
  public:
    void handle_sequence(VtSequence seq) override { sequences.push_back(std::move(seq)); }

    std::vector<VtSequence> sequences;
  };

  TestVtWriter writer;

  writer.write('\x1b');
  writer.write("[48;2");
  writer.write(";233;171;23m");

  REQUIRE(writer.sequences.size() == 1);
  CHECK(writer.sequences[0].type() == VtSequence::Type::CSI);
  CHECK(writer.sequences[0].content() == "48;2;233;171;23m");
}
