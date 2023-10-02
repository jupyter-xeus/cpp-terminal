/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/key.hpp"

#include "cpp-terminal/platforms/conversion.hpp"

// ------------------------- Key ---------------------------

void Term::Key::append_name(std::string& strOut) const
{
  Term::Key key = *this;
  if(key == Term::Key::NoKey) return;
  if(key.hasAlt())
  {
    strOut += "Alt+";
    key = static_cast<Term::Key>(key.value - static_cast<std::int32_t>(Term::MetaKey::Value::Alt));
  }
  if(key.hasCtrl())
  {
    strOut += "Ctrl+";
    if(!key.iscntrl()) key = static_cast<Term::Key>(key.value - static_cast<std::int32_t>(Term::MetaKey::Value::Ctrl));
  }
  if(key == Term::Key::Tab) strOut += "Tab";
  else if(key == Term::Key::Enter)
    strOut += "Enter";
  else if(key == Term::Key::Esc)
    strOut += "Esc";
  else if(key == Term::Key::Backspace)
    strOut += "Backspace";
  else if(key == Term::Key::Del)
    strOut += "Del";
  else if(key.iscntrl())
    strOut += static_cast<char>(key.value + 64);
  else if(key == Term::Key::Space)
    strOut += "Space";
  else if(key.isunicode()) { key.append_str(strOut); }
  else
  {
    switch(key)
    {
      case Term::Key::ArrowLeft: strOut += "Left Arrow"; break;
      case Term::Key::ArrowRight: strOut += "Right Arrow"; break;
      case Term::Key::ArrowUp: strOut += "Up arrow"; break;
      case Term::Key::ArrowDown: strOut += "Down arrow"; break;
      case Term::Key::Numeric5: strOut += "5 Numeric pad"; break;
      case Term::Key::Home: strOut += "Home"; break;
      case Term::Key::Insert: strOut += "Insert"; break;
      case Term::Key::End: strOut += "End"; break;
      case Term::Key::PageUp: strOut += "Page up"; break;
      case Term::Key::PageDown: strOut += "Page down"; break;
      case Term::Key::F1: strOut += "F1"; break;
      case Term::Key::F2: strOut += "F2"; break;
      case Term::Key::F3: strOut += "F3"; break;
      case Term::Key::F4: strOut += "F4"; break;
      case Term::Key::F5: strOut += "F5"; break;
      case Term::Key::F6: strOut += "F6"; break;
      case Term::Key::F7: strOut += "F7"; break;
      case Term::Key::F8: strOut += "F8"; break;
      case Term::Key::F9: strOut += "F9"; break;
      case Term::Key::F10: strOut += "F10"; break;
      case Term::Key::F11: strOut += "F11"; break;
      case Term::Key::F12: strOut += "F12"; break;
      case Term::Key::F13: strOut += "F13"; break;
      case Term::Key::F14: strOut += "F14"; break;
      case Term::Key::F15: strOut += "F15"; break;
      case Term::Key::F16: strOut += "F16"; break;
      case Term::Key::F17: strOut += "F17"; break;
      case Term::Key::F18: strOut += "F18"; break;
      case Term::Key::F19: strOut += "F19"; break;
      case Term::Key::F20: strOut += "F20"; break;
      case Term::Key::F21: strOut += "F21"; break;
      case Term::Key::F22: strOut += "F22"; break;
      case Term::Key::F23: strOut += "F23"; break;
      case Term::Key::F24: strOut += "F24"; break;
      case Term::Key::PrintScreen: strOut += "Print Screen"; break;
      case Term::Key::Menu: strOut += "Menu"; break;
      default: break;
    }
  }
}

void Term::Key::append_str(std::string& strOut) const
{
  if(!(this->value >= 0x10FFFFL)) { Term::Private::codepoint_to_utf8(strOut, static_cast<char32_t>(this->value)); }
}

std::string Term::Key::name() const
{
  std::string str;
  this->append_name(str);
  return str;
}

std::string Term::Key::str() const
{
  std::string str;
  this->append_str(str);
  return str;
}
