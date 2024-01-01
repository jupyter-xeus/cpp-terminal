/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/iostream.hpp"

#include <cpp-terminal/color.hpp>
#include <cpp-terminal/style.hpp>

int main()
{
  Term::cout << "Normal Text " << Term::Style::Bold << "Bold" << Term::Style::ResetBold << std::endl;
  Term::cout << "Normal Text " << Term::Style::Dim << "Dim" << Term::Style::ResetDim << std::endl;
  Term::cout << "Normal Text " << Term::Style::Italic << "Italic" << Term::Style::ResetItalic << std::endl;
  Term::cout << "Normal Text " << Term::Style::Underline << "Underline" << Term::Style::ResetUnderline << std::endl;
  Term::cout << "Normal Text " << Term::Style::Blink << "Blink" << Term::Style::ResetBlink << std::endl;
  Term::cout << "Normal Text " << Term::Style::BlinkRapid << "Blink Rapid" << Term::Style::ResetBlinkRapid << std::endl;
  Term::cout << "Normal Text " << Term::Style::Reversed << "Reversed" << Term::Style::ResetReversed << std::endl;
  Term::cout << "Normal Text " << Term::Style::Conceal << "Conceal" << Term::Style::ResetConceal << std::endl;
  Term::cout << "Normal Text " << Term::Style::Crossed << "Crossed" << Term::Style::ResetCrossed << std::endl;

  Term::cout << "Normal Text " << Term::Style::Font0 << "Font 0" << Term::Style::ResetFont << std::endl;
  Term::cout << "Normal Text " << Term::Style::Font1 << "Font 1" << Term::Style::ResetFont << std::endl;
  Term::cout << "Normal Text " << Term::Style::Font2 << "Font 2" << Term::Style::ResetFont << std::endl;
  Term::cout << "Normal Text " << Term::Style::Font3 << "Font 3" << Term::Style::ResetFont << std::endl;
  Term::cout << "Normal Text " << Term::Style::Font4 << "Font 4" << Term::Style::ResetFont << std::endl;
  Term::cout << "Normal Text " << Term::Style::Font5 << "Font 5" << Term::Style::ResetFont << std::endl;
  Term::cout << "Normal Text " << Term::Style::Font6 << "Font 6" << Term::Style::ResetFont << std::endl;
  Term::cout << "Normal Text " << Term::Style::Font7 << "Font 7" << Term::Style::ResetFont << std::endl;
  Term::cout << "Normal Text " << Term::Style::Font8 << "Font 8" << Term::Style::ResetFont << std::endl;
  Term::cout << "Normal Text " << Term::Style::Font9 << "Font 9" << Term::Style::ResetFont << std::endl;
  Term::cout << "Normal Text " << Term::Style::Font10 << "Font 10" << Term::Style::ResetFont << std::endl;

  Term::cout << "Normal Text " << Term::color_fg(Term::Color::Name::Red) << "colored" << Term::Style::DefaultForegroundColor << " back to default" << std::endl;
  Term::cout << "Normal Text " << Term::color_bg(Term::Color::Name::Red) << "colored" << Term::Style::DefaultBackgroundColor << " back to default" << std::endl;

  Term::cout << "Normal Text " << Term::Style::Frame << "Frame" << Term::Style::ResetFrame << std::endl;
  Term::cout << "Normal Text " << Term::Style::Encircle << "Encircle" << Term::Style::ResetEncircle << std::endl;
  Term::cout << "Normal Text " << Term::Style::Overline << "Overline" << Term::Style::ResetOverline << std::endl;

  Term::cout << "Normal Text " << Term::Style::BarRight << "Bar right" << Term::Style::ResetBar << std::endl;
  Term::cout << "Normal Text " << Term::Style::DoubleBarRight << "Double bar right" << Term::Style::ResetBar << std::endl;
  Term::cout << "Normal Text " << Term::Style::BarLeft << "Bar left" << Term::Style::ResetBar << std::endl;
  Term::cout << "Normal Text " << Term::Style::DoubleBarLeft << "Double bar left" << Term::Style::ResetBar << std::endl;
  Term::cout << "Normal Text " << Term::Style::StressMarking << "Stress Marking" << Term::Style::ResetBar << std::endl;

  Term::cout << "Normal Text " << Term::Style::Superscript << "Superscript" << Term::Style::ResetSuperscript << std::endl;
  Term::cout << "Normal Text " << Term::Style::Subscript << "Subscript" << Term::Style::ResetSubscript << std::endl;
  int val{0};
  Term::cin >> val;
}
