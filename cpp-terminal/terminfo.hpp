#pragma once

namespace Term
{

class Terminfo
{
public:
  // indicates the color mode (basically the original color resolution)
  // also used to manually override the original color resolution
  enum class ColorMode
  {
    Unset,
    // no color was used
    NoColor,
    // a 3bit color was used
    Bit3,
    // a 4bit color was used
    Bit4,
    // a 8bit color was used
    Bit8,
    // a 24bit (RGB) color was used
    Bit24,
  };
  Terminfo();
  static ColorMode getColorMode() { return m_colorMode; }
  bool             hasANSIEscapeCode();

private:
  void             setANSIEscapeCode();
  bool             m_ANSIEscapeCode{true};
  void             setColorMode();
  static ColorMode m_colorMode;
};

}  // namespace Term
