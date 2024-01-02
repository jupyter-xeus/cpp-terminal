/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#ifdef _WIN32
  #include <windows.h>
#endif

#include "cpp-terminal/cursor.hpp"
#include "cpp-terminal/private/env.hpp"
#include "cpp-terminal/private/file.hpp"
#include "cpp-terminal/terminfo.hpp"

#include <cstddef>
#include <string>

Term::Terminfo::ColorMode Term::Terminfo::m_colorMode{ColorMode::Unset};
Term::Terminfo::Booleans  Term::Terminfo::m_booleans{};
Term::Terminfo::Integers  Term::Terminfo::m_integers{};
Term::Terminfo::Strings   Term::Terminfo::m_strings{};

bool Term::Terminfo::get(const Term::Terminfo::Bool& key)
{
  check();
  return m_booleans[static_cast<std::size_t>(key)];
}

std::uint32_t Term::Terminfo::get(const Term::Terminfo::Integer& key)
{
  check();
  return m_integers[static_cast<std::size_t>(key)];
}

std::string Term::Terminfo::get(const Term::Terminfo::String& key)
{
  check();
  return m_strings[static_cast<std::size_t>(key)];
}

void Term::Terminfo::set(const Term::Terminfo::Bool& key, const bool& value) { m_booleans[static_cast<std::size_t>(key)] = value; }
void Term::Terminfo::set(const Term::Terminfo::Integer& key, const std::uint32_t& value) { m_integers[static_cast<std::size_t>(key)] = value; }
void Term::Terminfo::set(const Term::Terminfo::String& key, const std::string& value) { m_strings[static_cast<std::size_t>(key)] = value; }

#if defined(_WIN32)
bool WindowsVersionGreater(const DWORD& major, const DWORD& minor, const DWORD& patch)
{
  #if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4191)
  #else
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type"
  #endif
  NTSTATUS(WINAPI * getVersion)
  (PRTL_OSVERSIONINFOW) = (reinterpret_cast<NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW)>(GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "RtlGetVersion")));
  #if defined(_MSC_VER)
    #pragma warning(pop)
  #else
    #pragma GCC diagnostic pop
  #endif
  if(getVersion != nullptr)
  {
    RTL_OSVERSIONINFOW rovi;
    rovi.dwOSVersionInfoSize = sizeof(rovi);
    if(getVersion(&rovi) == 0)
    {
      if(rovi.dwMajorVersion > major || (rovi.dwMajorVersion == major && (rovi.dwMinorVersion > minor || (rovi.dwMinorVersion == minor && rovi.dwBuildNumber >= patch)))) return true;
      else
        return false;
    }
  }
  return false;
}
#endif

void Term::Terminfo::checkLegacy()
{
#if defined(_WIN32)
  #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
  #endif
  if(!m_booleans[static_cast<std::size_t>(Terminfo::Bool::ControlSequences)]) { set(Terminfo::Bool::Legacy, true); }
  else
  {
    DWORD dwOriginalOutMode{0};
    GetConsoleMode(Private::out.handle(), &dwOriginalOutMode);
    if(!SetConsoleMode(Private::out.handle(), dwOriginalOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) { set(Terminfo::Bool::Legacy, true); }
    else
    {
      SetConsoleMode(Private::out.handle(), dwOriginalOutMode);
      set(Terminfo::Bool::Legacy, false);
    }
  }
#else
  set(Terminfo::Bool::Legacy, false);
#endif
}

void Term::Terminfo::checkTermEnv() { set(Terminfo::String::TermEnv, Private::getenv("TERM").second); }

void Term::Terminfo::checkTerminalName()
{
  std::string name;
  name = Private::getenv("TERM_PROGRAM").second;
  if(name.empty()) { name = Private::getenv("TERMINAL_EMULATOR").second; }
  if(Private::getenv("ANSICON").first) { name = "ansicon"; }
  set(Term::Terminfo::String::TermName, name);
}

void Term::Terminfo::checkTerminalVersion() { set(Terminfo::String::TermVersion, Private::getenv("TERM_PROGRAM_VERSION").second); }

void Term::Terminfo::check()
{
  static bool checked{false};
  if(!checked)
  {
    checkTermEnv();
    checkTerminalName();
    checkTerminalVersion();
    checkControlSequences();
    checkLegacy();
    checkColorMode();
    checkUTF8();
    checked = true;
  }
}

Term::Terminfo::ColorMode Term::Terminfo::getColorMode()
{
  checkColorMode();
  return m_colorMode;
}

Term::Terminfo::Terminfo() { check(); }

void Term::Terminfo::checkColorMode()
{
  std::string name{m_strings[static_cast<std::size_t>(Terminfo::String::TermName)]};
  if(name == "Apple_Terminal") { m_colorMode = Term::Terminfo::ColorMode::Bit8; }
  else if(name == "JetBrains-JediTerm") { m_colorMode = Term::Terminfo::ColorMode::Bit24; }
  else if(name == "vscode") { m_colorMode = Term::Terminfo::ColorMode::Bit24; }
  else if(name == "linux") { m_colorMode = Term::Terminfo::ColorMode::Bit4; }
  else if(name == "ansicon") { m_colorMode = Term::Terminfo::ColorMode::Bit4; }
  else if(m_strings[static_cast<std::size_t>(Terminfo::String::TermEnv)] == "linux") { m_colorMode = Term::Terminfo::ColorMode::Bit4; }
#if defined(_WIN32)
  else if(WindowsVersionGreater(10, 0, 10586) && !m_booleans[static_cast<std::size_t>(Terminfo::Bool::Legacy)]) { m_colorMode = Term::Terminfo::ColorMode::Bit24; }
  else if(m_booleans[static_cast<std::size_t>(Terminfo::Bool::Legacy)]) { m_colorMode = Term::Terminfo::ColorMode::Bit4; }
#endif
  else { m_colorMode = Term::Terminfo::ColorMode::Bit24; }
  std::string colorterm = Private::getenv("COLORTERM").second;
  if((colorterm == "truecolor" || colorterm == "24bit") && m_colorMode != ColorMode::Unset) { m_colorMode = Term::Terminfo::ColorMode::Bit24; }
}

void Term::Terminfo::checkControlSequences()
{
#ifdef _WIN32
  if(WindowsVersionGreater(10, 0, 10586)) { set(Term::Terminfo::Bool::ControlSequences, true); }
  else { set(Term::Terminfo::Bool::ControlSequences, false); }
#else
  set(Term::Terminfo::Bool::ControlSequences, true);
#endif
}

void Term::Terminfo::checkUTF8()
{
#if defined(_WIN32)
  (GetConsoleOutputCP() == CP_UTF8 && GetConsoleCP() == CP_UTF8) ? set(Terminfo::Bool::UTF8, true) : set(Terminfo::Bool::UTF8, false);
#else
  Term::Cursor cursor_before{Term::cursor_position()};
  Term::Private::out.write("\xe2\x82\xac");  // € 3bits in utf8 one character
  std::string  read{Term::Private::in.read()};
  Term::Cursor cursor_after{Term::cursor_position()};
  std::size_t  moved{cursor_after.column() - cursor_before.column()};
  if(moved == 1) { set(Terminfo::Bool::UTF8, true); }
  else { set(Terminfo::Bool::UTF8, false); }
  for(std::size_t i = 0; i != moved; ++i) { Term::Private::out.write("\b \b"); }
#endif
}
