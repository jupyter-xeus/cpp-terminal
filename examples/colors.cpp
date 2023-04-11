#include "cpp-terminal/color.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/io.hpp"
#include "cpp-terminal/style.hpp"
#include "cpp-terminal/tty.hpp"
#include "cpp-terminal/version.hpp"

#include <iostream>

int main()
{
  std::cout << "Running cpp-terminal version: " << Term::Version << " website : " << Term::Homepage << std::endl << std::endl;

  try
  {
    if(Term::is_stdout_a_tty()) { std::cout << "Standard output is attached to a terminal." << std::endl << std::endl; }
    else { std::cout << "Standard output is not attached to a terminal." << std::endl << std::endl; }

    std::string mode;
    if(Term::Terminfo::getColorMode() == Term::Terminfo::ColorMode::Bit24) mode = "24bit";
    else if(Term::Terminfo::getColorMode() == Term::Terminfo::ColorMode::Bit8)
      mode = "8bit";
    else if(Term::Terminfo::getColorMode() == Term::Terminfo::ColorMode::Bit4)
      mode = "4bit";
    else if(Term::Terminfo::getColorMode() == Term::Terminfo::ColorMode::Bit3)
      mode = "3bit";
    else if(Term::Terminfo::getColorMode() == Term::Terminfo::ColorMode::NoColor)
      mode = "nocolor";
    else
      mode = "Unset";

    std::cout << "Terminal has " << mode << " color support" << std::endl << std::endl;

    std::string text = "Some text with " + Term::color_fg(Term::Color::Name::Red) + color_bg(Term::Color::Name::Green) + "red on green" + color_bg(Term::Color::Name::Default) + color_fg(Term::Color::Name::Default);
    text += " and some " + style(Term::Style::BOLD) + "bold text" + style(Term::Style::RESET) + ".\n";
    text += "Unicode works too: originally written by Ondřej Čertík.";
    std::cout << text << std::endl;

    std::string rgb_text = "Some Text in " + Term::color_fg(255, 0, 0) + 'R' + Term::color_fg(0, 255, 0) + 'G' + Term::color_fg(0, 0, 255) + 'B' + Term::color_fg(Term::Color::Name::Default);

    std::cout << rgb_text << std::endl;

    std::cout << "\n4bits colors:\n";
    std::cout << "*" << Term::color_bg(Term::Color::Name::Black) << " " << Term::color_bg(Term::Color::Name::Red) << " " << Term::color_bg(Term::Color::Name::Green) << " " << Term::color_bg(Term::Color::Name::Yellow) << " "
              << Term::color_bg(Term::Color::Name::Blue) << " " << Term::color_bg(Term::Color::Name::Magenta) << " " << Term::color_bg(Term::Color::Name::Cyan) << " " << Term::color_bg(Term::Color::Name::White) << " " << Term::color_bg(Term::Color::Name::Gray)
              << " " << Term::color_bg(Term::Color::Name::BrightRed) << " " << Term::color_bg(Term::Color::Name::BrightGreen) << " " << Term::color_bg(Term::Color::Name::BrightYellow) << " " << Term::color_bg(Term::Color::Name::BrightBlue) << " "
              << Term::color_bg(Term::Color::Name::BrightMagenta) << " " << Term::color_bg(Term::Color::Name::BrightCyan) << " " << Term::color_bg(Term::Color::Name::BrightWhite) << " " << Term::color_bg(Term::Color::Name::Default) << "*\n";

    std::cout << "\n8bits colors:\n";
    std::cout << "*";
    for(std::uint8_t i = 0; i < 255; i += 1) { std::cout << Term::color_bg(i) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n";

    std::cout << "\n24bits color chart: \n";
    std::cout << "*";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(i, 0, 0) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n*";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(0, i, 0) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n*";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(0, 0, i) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n*";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(i, i, i) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n";

    std::cout << "\nColor conversion (24bit)\n";
    /* red color space */
    std::cout << "24bit original: *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(i, 0, 0) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 8bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(i, 0, 0).to8bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 4bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(i, 0, 0).to4bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 3bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(i, 0, 0).to3bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    /* green color space */
    std::cout << "*\n24bit original: *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(0, i, 0) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 8bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(0, i, 0).to8bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 4bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(0, i, 0).to4bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 3bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(0, i, 0).to3bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    /* blue color space */
    std::cout << "*\n24bit original: *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(0, 0, i) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 8bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(0, 0, i).to8bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 4bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(0, 0, i).to4bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 3bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(0, 0, i).to3bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    /* black / grey color space */
    std::cout << "*\n24bit original: *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(i, i, i) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 8bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(i, i, i).to8bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 4bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(i, i, i).to4bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n24bit to 3bit:  *";
    for(std::uint8_t i = 0; i < 255; i += 3) { std::cout << Term::color_bg(Term::Color(i, i, i).to3bits()) << " " << Term::color_bg(Term::Color::Name::Default); }
    std::cout << "*\n";

    std::cout << "\nColor conversion (8bit)\n";
    std::cout << "to 4bit *";
    for(std::uint8_t i = 0; i < 255; i += 1) { std::cout << Term::color_bg(Term::Color(i).to4bits()) << " " << Term::color_bg(Term::Color::Name::Default) << ""; }
    std::cout << "*\n";
    std::cout << "to 3bit *";
    for(std::uint8_t i = 0; i < 255; i += 1) { std::cout << Term::color_bg(Term::Color(i).to3bits()) << " " << Term::color_bg(Term::Color::Name::Default) << ""; }
    std::cout << "*\n";

    std::cout << "\nColor conversion (4bit)\n";
    std::cout << "to 3bit : *" << Term::color_bg(Term::Color(Term::Color::Name::Black).to3bits()) << " " << Term::color_bg(Term::Color(Term::Color::Name::Red).to3bits()) << " " << Term::color_bg(Term::Color(Term::Color::Name::Green).to3bits()) << " "
              << Term::color_bg(Term::Color(Term::Color::Name::Yellow).to3bits()) << " " << Term::color_bg(Term::Color(Term::Color::Name::Blue).to3bits()) << " " << Term::color_bg(Term::Color(Term::Color::Name::Magenta).to3bits()) << " "
              << Term::color_bg(Term::Color(Term::Color::Name::Cyan).to3bits()) << " " << Term::color_bg(Term::Color(Term::Color::Name::White).to3bits()) << " " << Term::color_bg(Term::Color(Term::Color::Name::Gray).to3bits()) << " "
              << Term::color_bg(Term::Color(Term::Color::Name::BrightRed).to3bits()) << " " << Term::color_bg(Term::Color(Term::Color::Name::BrightGreen).to3bits()) << " " << Term::color_bg(Term::Color(Term::Color::Name::BrightYellow).to3bits()) << " "
              << Term::color_bg(Term::Color(Term::Color::Name::BrightBlue).to3bits()) << " " << Term::color_bg(Term::Color(Term::Color::Name::BrightMagenta).to3bits()) << " " << Term::color_bg(Term::Color(Term::Color::Name::BrightCyan).to3bits()) << " "
              << Term::color_bg(Term::Color(Term::Color::Name::BrightWhite).to3bits()) << " " << Term::color_bg(Term::Color::Name::Default) << " "
              << "*\n";

    std::cout << "Press any key to quit" << std::endl;
    if(Term::is_stdin_a_tty()) std::cin.get();
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
