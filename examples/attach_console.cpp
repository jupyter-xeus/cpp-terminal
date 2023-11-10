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
    std::cout << "Terminal has " << mode << " color support\n" << std::endl;

    std::string text = "Some text with " + Term::color_fg(Term::Color::Name::Red) + color_bg(Term::Color::Name::Green) + "red on green" + color_bg(Term::Color::Name::Default) + color_fg(Term::Color::Name::Default);
    text += " and some " + style(Term::Style::Bold) + "bold text" + style(Term::Style::Reset) + ".\n";
    text += "Unicode works too: originally written by Ondřej Čertík.";
    std::cout << text << std::endl;

    Term::cout<<"Now some utf8 ∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i)γνωρίζω ἀπὸ τὴν ὄψηდარგებში⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙როგორიცააᚻ∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂᛖᛒᚢᛞᛖразличных\tопеฮั่นเสื่อมโทรมแማደሪያ የለው፥ ግንድ ይዞ ይዞራል።"<<std::endl;


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
