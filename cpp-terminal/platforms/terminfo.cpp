#ifdef _WIN32
  #include <windows.h>
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
#endif

#include "cpp-terminal/terminfo.hpp"
#include <string>


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
}
#endif

std::string getenv(const std::string& env)
{
#ifdef _WIN32
  std::size_t requiredSize{0};
  getenv_s(&requiredSize, nullptr, 0, env.c_str());
  if(requiredSize == 0) return std::string();
  std::string ret;
  ret.reserve(requiredSize * sizeof(char));
  getenv_s(&requiredSize, &ret[0], requiredSize, env.c_str());
#else
  if(std::getenv(env.c_str()) != nullptr) return static_cast<std::string>(std::getenv(env.c_str()));
  else
    return std::string();
#endif
}

Term::Terminfo::ColorMode Term::Terminfo::m_colorMode{Term::Terminfo::ColorMode::Unset};

Term::Terminfo::Terminfo() { setANSIEscapeCode();
  setColorMode();
}

bool Term::Terminfo::hasANSIEscapeCode() { return m_ANSIEscapeCode; }

void Term::Terminfo::setColorMode()
{
  std::string colorterm = getenv("COLORTERM");
  if(colorterm == "truecolor" || colorterm == "24bit") m_colorMode = Term::Terminfo::ColorMode::Bit24;
  else
    m_colorMode = Term::Terminfo::ColorMode::Bit8;
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
