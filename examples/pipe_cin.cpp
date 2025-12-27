/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/color.hpp"
#include "cpp-terminal/terminal.hpp"

#include <iostream>
#include <string>
#include <sys/select.h>
#include <unistd.h>

namespace
{

bool isStdinEmpty()
{
  struct timeval tv
  {
    0, 0
  };
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(STDIN_FILENO, &rfds);
  return select(STDIN_FILENO + 1, &rfds, nullptr, nullptr, &tv) == 0;
}

std::string ReadStdin()
{
  if(isStdinEmpty()) { return "stdin is empty"; }
  std::string stdin_result;
  char        c;
  while((c = static_cast<char>(std::fgetc(stdin))) != EOF)
  {
    if(c == '\n') break;
    stdin_result += c;
  }
  return stdin_result;
}

bool isCinPipe() { return isatty(fileno(stdin)) == 0; }

std::string ReadCin()
{
  if(!isCinPipe()) { return "std::cin isn't an unamed pipe"; }
  std::string cin_result;
  char        c;
  while((c = static_cast<char>(std::cin.get())) != '\n') { cin_result += c; }
  return cin_result;
}

std::ostream& PrintOutColor(std::ostream& ostr, std::string const& str, Term::Color::Name color)
{
  ostr << "\"" << Term::color_fg(color) << str << Term::color_fg(Term::Color::Name::Default) << "\"" << std::endl;
  return ostr;
}

}  // namespace

int main()
{
  std::string cinBuff{};

  std::cout << "Checking if std::cin is empty" << std::endl;
  cinBuff = ReadCin();
  std::cout << "std::cin => ";
  PrintOutColor(std::cout, cinBuff, Term::Color::Name::Red);

  std::cout << "Checking if stdin is empty" << std::endl;
  cinBuff = ReadStdin();
  std::cout << "stdin    => ";
  PrintOutColor(std::cout, cinBuff, Term::Color::Name::Red);

  return 0;
}
