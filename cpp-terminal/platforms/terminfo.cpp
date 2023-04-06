#ifdef _WIN32
  #include <windows.h>
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
#endif

#include "cpp-terminal/terminfo.hpp"

#include <string>
#include <utility>

#ifdef _WIN32
bool WindowsVersionGreater(const DWORD& major, const DWORD& minor, const DWORD& patch)
{
  RtlGetVersionPtr fn = {reinterpret_cast<RtlGetVersionPtr>(GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "RtlGetVersion"))};
  if(fn != nullptr)
  {
    RTL_OSVERSIONINFOW rovi;
    rovi.dwOSVersionInfoSize = sizeof(rovi);
    if(fn(&rovi) == 0)
    {
      if(rovi.dwMajorVersion > major || (rovi.dwMajorVersion == major && (rovi.dwMinorVersion > minor || (rovi.dwMinorVersion == minor && rovi.dwBuildNumber >= patch)))) return true;
      else
        return false;
    }
  }
  return false;
}
#endif

namespace Private
{
std::pair<bool, std::string> getenv(const std::string& env)
{
#ifdef _WIN32
  std::size_t requiredSize{0};
  getenv_s(&requiredSize, nullptr, 0, env.c_str());
  if(requiredSize == 0) return {false, std::string()};
  std::string ret;
  ret.reserve(requiredSize * sizeof(char));
  getenv_s(&requiredSize, &ret[0], requiredSize, env.c_str());
  return {true, ret};
#else
  if(std::getenv(env.c_str()) != nullptr) return {true, static_cast<std::string>(std::getenv(env.c_str()))};
  else
    return {false, std::string()};
#endif
}
}  // namespace Private

Term::Terminfo::ColorMode Term::Terminfo::m_colorMode{Term::Terminfo::ColorMode::Unset};

Term::Terminfo::Terminfo()
{
  m_term         = Private::getenv("TERM").second;
  m_terminalName = Private::getenv("TERM_PROGRAM").second;
  m_terminalName = Private::getenv("TERMINAL_EMULATOR").second;
  if(Private::getenv("ANSICON").first) m_terminalName = "ansicon";
  m_terminalVersion = Private::getenv("TERM_PROGRAM_VERSION").second;
  setANSIEscapeCode();
  setColorMode();
}

bool Term::Terminfo::hasANSIEscapeCode() { return m_ANSIEscapeCode; }

void Term::Terminfo::setColorMode()
{
  std::string colorterm = Private::getenv("COLORTERM").second;
  if(colorterm == "truecolor" || colorterm == "24bit") m_colorMode = Term::Terminfo::ColorMode::Bit24;
  else
    m_colorMode = Term::Terminfo::ColorMode::Bit8;
  if(m_terminalName == "Apple_Terminal") m_colorMode = Term::Terminfo::ColorMode::Bit8;
  else if(m_terminalName == "JetBrains-JediTerm")
    m_colorMode = Term::Terminfo::ColorMode::Bit24;
  else if(m_terminalName == "vscode")
    m_colorMode = Term::Terminfo::ColorMode::Bit24;
#ifdef _WIN32
  if(WindowsVersionGreater(10, 0, 10586)) m_colorMode = Term::Terminfo::ColorMode::Bit24;
  else if(m_terminalName == "ansicon")
    m_colorMode = Term::Terminfo::ColorMode::Bit4;
  else
    m_colorMode = Term::Terminfo::ColorMode::Bit4;
#endif
}

void Term::Terminfo::setANSIEscapeCode()
{
#ifdef _WIN32
  if(WindowsVersionGreater(10, 0, 10586)) m_ANSIEscapeCode = true;
  else
    m_ANSIEscapeCode = false;
#else
  m_ANSIEscapeCode = true;
#endif
}
