/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2026 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/color.hpp"
#include "cpp-terminal/iostream.hpp"
#include "cpp-terminal/terminal.hpp"
#include "cpp-terminal/win_adapter.hpp"

#include <cpp-terminal/style.hpp>
#include <iostream>
#include <limits>

int main()
{
  std::string rgb_text                = Term::color_fg(std::numeric_limits<std::uint8_t>::max(), 0, 0) + 'R' + Term::color_fg(0, std::numeric_limits<std::uint8_t>::max(), 0) + 'G' + Term::color_fg(0, 0, std::numeric_limits<std::uint8_t>::max()) + 'B' + Term::color_fg(Term::Color::Name::Default);
  std::string enable_error_reporting  = "\x1b[?3210h";
  std::string disable_error_reporting = "\x1b[?3210l";
  std::string cursor_out_of_bounds    = "\x1b[250G";

  Term::cout << "Microsoft didn't add support for ANSI escape sequences to it's console until Windows 10. ";
  Term::cout << "Prior to that, ANSI commands written to output streams were treated as normal text. ";
  Term::cout << "To remedy that, CPP-Terminal provides ANSI support for older versions of Windows beginning with Windows 2000.\n\n";

  Term::cout << "If CPP-Terminal detects it's running on an older versions of Windows, it will automatically ";
  Term::cout << "extract ANSI sequences from it's output streams (Term::cout, Term::cerr and Term::clog) and ";
  Term::cout << "call the appropriate Win32 console API methods to achieve the desired result.\n\n";

  Term::cout << "To prevent this default behavior, set the 'NoLegacyWindowsSupport' option:\n" << std::endl;
  Term::cout << Term::Style::Bold << "  Term::terminal.setOptions(Term::Option::NoLegacyWindowsSupport);" << Term::Style::ResetBold << std::endl;

  Term::cout << "\nOr, to enable this behavior for std::cout, std::cerr and/or std::clog, simply construct a WinConsoleAdapter ";
  Term::cout << "with the desired stream. Conversion of ANSI escape sequences will continue until the object is destroyed.\n" << std::endl;
  Term::cout << "  " << Term::Style::Bold << "Term::WinConsoleAdapter adapter(std::cout);" << Term::Style::ResetBold << std::endl;

  Term::cout << "\nIf this example is running on a pre-Win 10 machine, you'll either see a red R, green G and blue B ";
  Term::cout << "or a string of ANSI commands, depending on what support options are active. ";
  Term::cout << "Otherwise, you'll see all the following as the colored letters 'RGB'.\n\n";

  Term::cout << "No ANSI support:" << std::endl;
  Term::terminal.setOptions(Term::Option::NoLegacyWindowsSupport);
  Term::cout << "Term::cout (ANSI):\t" << rgb_text << std::endl;
  std::cout << "std::cout (ANSI):\t" << rgb_text << std::endl;

  Term::cout << "\nTerm::cxx support:" << std::endl;
  Term::terminal.setOptions(Term::Option::LegacyWindowsSupport);
  Term::cout << "Term::cout (RGB):\t" << rgb_text << std::endl;
  std::cout << "std::cout (ANSI):\t" << rgb_text << std::endl;

  Term::cout << "\nstd::cout support:" << std::endl;
  Term::WinConsoleAdapter adapter(std::cout);
  Term::cout << "Term::cout (RGB):\t" << rgb_text << std::endl;
  std::cout << "std::cout (RGB):\t" << rgb_text << std::endl;

  Term::cout << "\nIf a WinConsoleAdapter is constructed in a modern Windows or non-Windows environment, it does nothing ";
  Term::cout << "so it's safe to use without testing for a legacy Windows environment beforehand." << std::endl;

  Term::cout << "\nNormally, any parsing errors or Windows API call failures are silently ignored. However, for ";
  Term::cout << "troubleshooting purposes error reporting can be enabled. If enabled, any errors are printed to the ";
  Term::cout << "terminal.\n\n";
  Term::cout << "Error reporting is enabled with the custom sequence '" << Term::Style::Bold << "CSI ? 3210h" << Term::Style::ResetBold;
  Term::cout << "' and disabled with '" << Term::Style::Bold << "CSI ? 3210l" << Term::Style::ResetBold << "'.\n\n";
  Term::cout << "For example, this is the error reported for an attempt to move the cursor beyond the bounds of the screen ";
  Term::cout << "(for those of you with a legacy console, the error message will be printed twice).\n";
  Term::cout << "\n---ERROR---> Requested cursor position is outside screen buffer. Command type 'CursorHorizontalAbsolute' params: {250} intermediates: none\n";
  Term::cout << enable_error_reporting << cursor_out_of_bounds << std::endl;

  return 0;
}
