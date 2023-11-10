/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/color.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/style.hpp"
#include "cpp-terminal/version.hpp"

#ifdef _WIN32
  #include <windows.h>
#endif

#include <iostream>
#include <limits>

#ifdef _WIN32
int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#else
int main()
#endif
{
  try
  {
    std::string mode;
    if(Term::Terminfo::getColorMode() == Term::Terminfo::ColorMode::Bit24) { mode = "24bit"; }
    else if(Term::Terminfo::getColorMode() == Term::Terminfo::ColorMode::Bit8) { mode = "8bit"; }
    else if(Term::Terminfo::getColorMode() == Term::Terminfo::ColorMode::Bit4) { mode = "4bit"; }
    else if(Term::Terminfo::getColorMode() == Term::Terminfo::ColorMode::Bit3) { mode = "3bit"; }
    else if(Term::Terminfo::getColorMode() == Term::Terminfo::ColorMode::NoColor) { mode = "nocolor"; }
    else { mode = "Unset"; }

    std::cout << "Terminal has " << mode << " color support" << std::endl << std::endl;

    std::string text = "Some text with " + Term::color_fg(Term::Color::Name::Red) + color_bg(Term::Color::Name::Green) + "red on green" + color_bg(Term::Color::Name::Default) + color_fg(Term::Color::Name::Default);
    text += " and some " + style(Term::Style::Bold) + "bold text" + style(Term::Style::Reset) + ".\n";
    text += "Unicode works too: originally written by Ondřej Čertík.";
    std::cout << text << std::endl;

    std::string rgb_text = "Some Text in " + Term::color_fg(std::numeric_limits<std::uint8_t>::max(), 0, 0) + 'R' + Term::color_fg(0, std::numeric_limits<std::uint8_t>::max(), 0) + 'G' + Term::color_fg(0, 0, std::numeric_limits<std::uint8_t>::max()) + 'B' + Term::color_fg(Term::Color::Name::Default);

    std::cout << rgb_text << std::endl;

    std::cout << "\n4bits colors:\n";
    std::cout << "*" << Term::color_bg(Term::Color::Name::Black) << " " << Term::color_bg(Term::Color::Name::Red) << " " << Term::color_bg(Term::Color::Name::Green) << " " << Term::color_bg(Term::Color::Name::Yellow) << " " << Term::color_bg(Term::Color::Name::Blue) << " " << Term::color_bg(Term::Color::Name::Magenta) << " " << Term::color_bg(Term::Color::Name::Cyan) << " " << Term::color_bg(Term::Color::Name::White) << " "
              << Term::color_bg(Term::Color::Name::Gray) << " " << Term::color_bg(Term::Color::Name::BrightRed) << " " << Term::color_bg(Term::Color::Name::BrightGreen) << " " << Term::color_bg(Term::Color::Name::BrightYellow) << " " << Term::color_bg(Term::Color::Name::BrightBlue) << " " << Term::color_bg(Term::Color::Name::BrightMagenta) << " " << Term::color_bg(Term::Color::Name::BrightCyan) << " "
              << Term::color_bg(Term::Color::Name::BrightWhite) << " " << Term::color_bg(Term::Color::Name::Default) << "*\n";

    std::cout << "\n8bits colors:\n";
    std::cout << "*";
    for(std::uint8_t i = 0; i < std::numeric_limits<std::uint8_t>::max(); i += 1) { std::cout << Term::color_bg(i) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n";

    std::cout << "\n24bits color chart: \n";
    std::cout << "*";
    for(std::uint8_t i = 0; i < std::numeric_limits<std::uint8_t>::max(); i += 3) { std::cout << Term::color_bg(i, 0, 0) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n*";
    for(std::uint8_t i = 0; i < std::numeric_limits<std::uint8_t>::max(); i += 3) { std::cout << Term::color_bg(0, i, 0) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n*";
    for(std::uint8_t i = 0; i < std::numeric_limits<std::uint8_t>::max(); i += 3) { std::cout << Term::color_bg(0, 0, i) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n*";
    for(std::uint8_t i = 0; i < std::numeric_limits<std::uint8_t>::max(); i += 3) { std::cout << Term::color_bg(i, i, i) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n";

#ifdef _WIN32
    MessageBox(NULL, "Hello, world", "caption", 0);
#endif
  }
  catch(const Term::Exception& re)
  {
    std::cerr << "cpp-terminal error: " << re.what() << std::endl;
    return 2;
  }
  catch(...)
  {
    std::cerr << "Unknown error." << std::endl;
    return 1;
  }
  return 0;
}
