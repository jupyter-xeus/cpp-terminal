#include "cpp-terminal/color.hpp"

#include "cpp-terminal/terminfo.hpp"

bool Term::Color::operator==(const Term::Color& color) const
{
  if(color.getType() != getType()) return false;
  if(getType() == Term::Color::Type::Bit24) return m_bit24 == color.to24bits();
  else
    return m_bit8 == color.to8bits();
}

bool Term::Color::operator!=(const Term::Color& color) const { return !(*this == color); }

Term::Color::Color() : m_bit8(0) {}

Term::Color::Color(const Term::Color::Name& name) : m_Type(Type::Bit4), m_bit8(static_cast<std::uint8_t>(name)) {}

Term::Color::Color(const std::uint8_t& color) : m_Type(Type::Bit8), m_bit8(color) {}

Term::Color::Color(const std::uint8_t& r, const std::uint8_t& b, const std::uint8_t& g) : m_Type(Type::Bit24)
{
  // Hack for gcc4.7
  m_bit24[0] = r;
  m_bit24[1] = b;
  m_bit24[2] = g;
}

Term::Color::Type Term::Color::getType() const { return m_Type; }

Term::Color::Name Term::Color::to3bits() const
{
  if(getType() == Type::Bit3) return static_cast<Term::Color::Name>(m_bit8);
  else
  {
    Term::Color::Name ret{to4bits()};
    if(ret >= Term::Color::Name::Gray) return static_cast<Term::Color::Name>(static_cast<uint8_t>(ret) - static_cast<uint8_t>(Term::Color::Name::Gray));
    else
      return ret;
  }
}

// Convert 24bits and 8bits to 4bits
Term::Color::Name Term::Color::to4bits() const
{
  //https://ajalt.github.io/colormath/converter/
  // clang-format off
  static const std::array<std::uint8_t,256> table ={
    0,  1,  2,  3,  4,  5,  6,  7,  60, 61, 62, 63, 64, 65, 66, 67, 0,  4,  4,  4,  64, 64, 2,  6,  4,  4,  64, 64, 2,  2,  6,  4,  64, 64, 2,  2,  2,  6,  64, 64, 62, 62, 62, 62, 66, 64, 62, 62, 62, 62, 62, 66,
    1,  5,  4,  4,  64, 64, 3,  60, 4,  4,  64, 64, 2,  2,  6,  4,  64, 64, 2,  2,  2,  6,  64, 64, 62, 62, 62, 62, 66, 64, 62, 62, 62, 62, 62, 66, 1,  1,  5,  4,  64, 64, 1,  1,  5,  4,  64, 64, 3,  3,  60, 4,
    64, 64, 2,  2,  2,  6,  64, 64, 62, 62, 62, 62, 66, 64, 62, 62, 62, 62, 62, 66, 1,  1,  1,  5,  64, 64, 1,  1,  1,  5,  64, 64, 1,  1,  1,  5,  64, 64, 3,  3,  3,  7,  64, 64, 62, 62, 62, 62, 66, 64, 62, 62,
    62, 62, 62, 66, 61, 61, 61, 61, 65, 64, 61, 61, 61, 61, 65, 64, 61, 61, 61, 61, 65, 64, 61, 61, 61, 61, 65, 64, 63, 63, 63, 63, 7,  64, 62, 62, 62, 62, 62, 66, 61, 61, 61, 61, 61, 65, 61, 61, 61, 61, 61, 65,
    61, 61, 61, 61, 61, 65, 61, 61, 61, 61, 61, 65, 61, 61, 61, 61, 61, 65, 63, 63, 63, 63, 63, 67, 0,  0,  0,  0,  0,  0,  60, 60, 60, 60, 60, 60, 7,  7,  7,  7,  7,  7,  67, 67, 67, 67, 67, 67};
  // clang-format on
  if(getType() == Term::Color::Type::Bit24 || getType() == Term::Color::Type::Bit8) { return static_cast<Term::Color::Name>(table[to8bits()]); }
  else
    return static_cast<Term::Color::Name>(m_bit8);
}

// Convert 24bits to 8bits
std::uint8_t Term::Color::to8bits() const
{
  if(getType() == Term::Color::Type::Bit24)
  {
    // check gray scale in 24 steps
    if(m_bit24[0] == m_bit24[1] && m_bit24[0] == m_bit24[2]) { return 232 + m_bit24[0] / 32 + m_bit24[1] / 32 + m_bit24[2] / 32; }
    // normal color space
    return 16 + 36 * (m_bit24[0] / 51) + 6 * (m_bit24[1] / 51) + (m_bit24[2] / 51);
  }
  else
    return m_bit8;
}

// Nothing to do
std::array<std::uint8_t, 3> Term::Color::to24bits() const { return m_bit24; }

std::string Term::color_bg(const Term::Color::Name& color) { return color_bg(Color(color)); }
void        Term::color_bg(std::string& stringOut, const Term::Color::Name& color) { color_bg(stringOut, Color(color)); }

std::string Term::color_bg(const std::uint8_t& color) { return color_bg(Color(color)); }
void        Term::color_bg(std::string& stringOut, const std::uint8_t& color) { color_bg(stringOut, Color(color)); }

std::string Term::color_bg(const std::uint8_t& r, const std::uint8_t& g, const std::uint8_t& b) { return color_bg(Color(r, g, b)); }
void        Term::color_bg(std::string& stringOut, const std::uint8_t& r, const std::uint8_t& g, const std::uint8_t& b) { color_bg(stringOut, Color(r, g, b)); }

std::string Term::color_bg(const Color& color)
{
  std::string result;
  color_bg(result, color);
  return result;
}

//https://unix.stackexchange.com/questions/212933/background-color-whitespace-when-end-of-the-terminal-reached
//FIX maybe we need an other function without [K if we want to modify background of part of the screen (Moving cursor and changing color )
void Term::color_bg(std::string& stringOut, const Color& color)
{
  if(color.getType() == Term::Color::Type::Unset || color.getType() == Term::Color::Type::NoColor) return;
  else
    switch(Term::Terminfo::getColorMode())
    {
      case Term::Terminfo::ColorMode::Unset:
      case Term::Terminfo::ColorMode::NoColor: return;

      case Term::Terminfo::ColorMode::Bit3:
        stringOut.append("\033[", sizeof("\033[") - 1);
        stringOut.append(std::to_string(static_cast<uint8_t>(color.to3bits()) + 40));
        stringOut.append("m\033[K", sizeof("m\033[K") - 1);
        return;

      case Term::Terminfo::ColorMode::Bit4:
        stringOut.append("\033[", sizeof("\033[") - 1);
        stringOut.append(std::to_string(static_cast<uint8_t>(color.to4bits()) + 40));
        stringOut.append("m\033[K", sizeof("m\033[K") - 1);
        return;

      case Term::Terminfo::ColorMode::Bit8:
        if(color.getType() == Term::Color::Type::Bit4 || color.getType() == Term::Color::Type::Bit3)
        {
          stringOut.append("\033[", sizeof("\033[") - 1);
          stringOut.append(std::to_string(static_cast<uint8_t>(color.to4bits()) + 40));
          stringOut.append("m\033[K", sizeof("m\033[K") - 1);
          return;
        }
        else
        {
          stringOut.append("\033[48;5;", sizeof("\033[48;5;") - 1);
          stringOut.append(std::to_string(static_cast<uint8_t>(color.to4bits()) + 40));
          stringOut.append("m\033[K", sizeof("m\033[K") - 1);
          return;
        }

      case Term::Terminfo::ColorMode::Bit24:
        if(color.getType() == Term::Color::Type::Bit3 || color.getType() == Term::Color::Type::Bit4)
        {
          stringOut.append("\033[", sizeof("\033[") - 1);
          stringOut.append(std::to_string(static_cast<uint8_t>(color.to4bits()) + 40));
          stringOut.append("m\033[K", sizeof("m\033[K") - 1);
          return;
        }
        else if(color.getType() == Term::Color::Type::Bit8)
        {
          stringOut.append("\033[48;5;", sizeof("\033[48;5;") - 1);
          stringOut.append(std::to_string(color.to8bits()));
          stringOut.append("m\033[K", sizeof("m\033[K") - 1);
          return;
        }
        else
        {
          stringOut.append("\033[48;2;", sizeof("\033[48;2;") - 1);
          stringOut.append(std::to_string(color.to24bits()[0]));
          stringOut.push_back(';');
          stringOut.append(std::to_string(color.to24bits()[1]));
          stringOut.push_back(';');
          stringOut.append(std::to_string(color.to24bits()[2]));
          stringOut.append("m\033[K", sizeof("m\033[K") - 1);
          return;
        }
    }
  return;
}

std::string Term::color_fg(const Term::Color::Name& color) { return color_fg(Color(color)); }
void        Term::color_fg(std::string& outputString, const Term::Color::Name& color) { return color_fg(outputString, Color(color)); }

std::string Term::color_fg(const std::uint8_t& color) { return color_fg(Color(color)); }
void        Term::color_fg(std::string& outputString, const std::uint8_t& color) { return color_fg(outputString, Color(color)); }

std::string Term::color_fg(const std::uint8_t& r, const std::uint8_t& g, const std::uint8_t& b) { return color_fg(Color(r, g, b)); }
void        Term::color_fg(std::string& outputString, const std::uint8_t& r, const std::uint8_t& g, const std::uint8_t& b) { return color_fg(outputString, Color(r, g, b)); }

std::string Term::color_fg(const Color& color)
{
  std::string s;
  color_fg(s, color);
  return s;
}

void Term::color_fg(std::string& outputString, const Color& color)
{
  if(color.getType() == Term::Color::Type::Unset || color.getType() == Term::Color::Type::NoColor) { return; }
  else
  {
    switch(Term::Terminfo::getColorMode())
    {
      case Term::Terminfo::ColorMode::Unset:
      case Term::Terminfo::ColorMode::NoColor: return;

      case Term::Terminfo::ColorMode::Bit3:
        outputString.append("\033[", sizeof("\033[") - 1);
        outputString.append(std::to_string(static_cast<uint8_t>(color.to3bits()) + 30));
        outputString.push_back('m');
        return;

      case Term::Terminfo::ColorMode::Bit4:
        outputString.append("\033[", sizeof("\033[") - 1);
        outputString.append(std::to_string(static_cast<uint8_t>(color.to4bits()) + 30));
        outputString.push_back('m');
        return;

      case Term::Terminfo::ColorMode::Bit8:
        if(color.getType() == Term::Color::Type::Bit4 || color.getType() == Term::Color::Type::Bit3)
        {
          outputString.append("\033[", sizeof("\033[") - 1);
          outputString.append(std::to_string(static_cast<uint8_t>(color.to4bits()) + 30));
          outputString.push_back('m');
          return;
        }
        else
        {
          outputString.append("\033[38;5;", sizeof("\033[38;5;") - 1);
          outputString.append(std::to_string(color.to8bits()));
          outputString.push_back('m');
          return;
        }

      case Term::Terminfo::ColorMode::Bit24:
        if(color.getType() == Term::Color::Type::Bit3 || color.getType() == Term::Color::Type::Bit4)
        {
          outputString.append("\033[", sizeof("\033[") - 1);
          outputString.append(std::to_string(static_cast<uint8_t>(color.to4bits()) + 30));
          outputString.push_back('m');
          return;
        }
        else if(color.getType() == Term::Color::Type::Bit8)
        {
          outputString.append("\033[38;5;", sizeof("\033[38;5;") - 1);
          outputString.append(std::to_string(color.to8bits()));
          outputString.push_back('m');
          return;
        }
        else
        {
          outputString.append("\033[38;2;", sizeof("\033[38;2;") - 1);
          outputString.append(std::to_string(color.to24bits()[0]));
          outputString.push_back(';');
          outputString.append(std::to_string(color.to24bits()[1]));
          outputString.push_back(';');
          outputString.append(std::to_string(color.to24bits()[2]));
          outputString.push_back('m');
          return;
        }
    }
  }
}
