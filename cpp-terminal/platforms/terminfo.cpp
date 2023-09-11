#ifdef _WIN32
  #include "cpp-terminal/platforms/file.hpp"

  #include <windows.h>
#endif

#include "cpp-terminal/platforms/env.hpp"
#include "cpp-terminal/terminfo.hpp"

#include <string>

#if defined(_WIN32)
bool WindowsVersionGreater(const DWORD& major, const DWORD& minor, const DWORD& patch)
{
  #if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4191)
  #else
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wuninitialized"
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

void Term::Terminfo::setLegacy()
{
#if defined(_WIN32)
  #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
  #endif
  if(!hasANSIEscapeCode()) m_legacy = true;
  else
  {
    DWORD dwOriginalOutMode{0};
    Term::Private::m_fileInitializer.init();  //Just in case
    GetConsoleMode(Private::out.handle(), &dwOriginalOutMode);
    if(!SetConsoleMode(Private::out.handle(), dwOriginalOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) m_legacy = true;
    else
    {
      SetConsoleMode(Private::out.handle(), dwOriginalOutMode);
      m_legacy = false;
    }
  }
#else
  m_legacy         = false;
#endif
}

Term::Terminfo::ColorMode Term::Terminfo::getColorMode() { return m_colorMode; }

bool Term::Terminfo::isLegacy() const { return m_legacy; }

Term::Terminfo::ColorMode Term::Terminfo::m_colorMode{ColorMode::Unset};

Term::Terminfo::Terminfo()
{
  m_term         = Private::getenv("TERM").second;
  m_terminalName = Private::getenv("TERM_PROGRAM").second;
  if(m_terminalName.empty()) m_terminalName = Private::getenv("TERMINAL_EMULATOR").second;
  if(Private::getenv("ANSICON").first) m_terminalName = "ansicon";
  m_terminalVersion = Private::getenv("TERM_PROGRAM_VERSION").second;
  setANSIEscapeCode();
  setLegacy();
  setColorMode();
}

bool Term::Terminfo::hasANSIEscapeCode() const { return m_ANSIEscapeCode; }

void Term::Terminfo::setColorMode()
{
  if(m_terminalName == "Apple_Terminal") m_colorMode = Term::Terminfo::ColorMode::Bit8;
  else if(m_terminalName == "JetBrains-JediTerm")
    m_colorMode = Term::Terminfo::ColorMode::Bit24;
  else if(m_terminalName == "vscode")
    m_colorMode = Term::Terminfo::ColorMode::Bit24;
  else if(m_terminalName == "linux")
    m_colorMode = Term::Terminfo::ColorMode::Bit4;
  else if(m_terminalName == "ansicon")
    m_colorMode = Term::Terminfo::ColorMode::Bit4;
  else if(m_term == "linux")
    m_colorMode = Term::Terminfo::ColorMode::Bit4;
#if defined(_WIN32)
  else if(WindowsVersionGreater(10, 0, 10586) && !isLegacy())
    m_colorMode = Term::Terminfo::ColorMode::Bit24;
  else if(isLegacy())
    m_colorMode = Term::Terminfo::ColorMode::Bit4;
#endif
  else
    m_colorMode = Term::Terminfo::ColorMode::Bit24;
  std::string colorterm = Private::getenv("COLORTERM").second;
  if((colorterm == "truecolor" || colorterm == "24bit") && m_colorMode != ColorMode::Unset) m_colorMode = Term::Terminfo::ColorMode::Bit24;
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
