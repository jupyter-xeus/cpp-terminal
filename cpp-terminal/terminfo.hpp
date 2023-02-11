#pragma once

namespace Term
{

class Terminfo
{
public:
  Terminfo();
  bool hasANSIEscapeCode();
private:
  void setANSIEscapeCode();
  bool m_ANSIEscapeCode{true};
};

}