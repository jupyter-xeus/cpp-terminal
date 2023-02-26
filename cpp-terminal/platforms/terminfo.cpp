#ifdef _WIN32
  #include <windows.h>
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
#endif

#include "cpp-terminal/terminfo.hpp"

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

Term::Terminfo::Terminfo() { setANSIEscapeCode(); }

bool Term::Terminfo::hasANSIEscapeCode() { return m_ANSIEscapeCode; }

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
