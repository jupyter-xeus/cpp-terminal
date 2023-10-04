#include "cpp-terminal/platforms/unicode.hpp"

#if defined(_WIN32)
  #include <windows.h>
#endif

#include "cpp-terminal/exception.hpp"

#if defined(_WIN32)
std::wstring Term::Private::utf8_to_wide(const std::string& utf8)
{
  if(utf8.empty()) return std::wstring();
  static constexpr DWORD kFlags{MB_ERR_INVALID_CHARS};
  if(utf8.length() > static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::overflow_error("Input string too long: size_t-length doesn't fit into int.");
  const int utf16Length{::MultiByteToWideChar(CP_UTF8, kFlags, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0)};
  if(utf16Length == 0)
  {
    const DWORD error{::GetLastError()};
    throw Term::Exception(error, error == ERROR_NO_UNICODE_TRANSLATION ? "Invalid UTF-8 sequence found in input string." : "Cannot get result string length when converting from UTF-8 to UTF-16 (MultiByteToWideChar failed).");
  }
  std::wstring utf16;
  utf16.reserve(utf16Length);
  int result{::MultiByteToWideChar(CP_UTF8, kFlags, utf8.data(), static_cast<int>(utf8.size()), &utf16[0], utf16Length)};
  if(result == 0)
  {
    const DWORD error{::GetLastError()};
    throw Term::Exception(error, error == ERROR_NO_UNICODE_TRANSLATION ? "Invalid UTF-8 sequence found in input string." : "Cannot convert from UTF-8 to UTF-16 (MultiByteToWideChar failed).");
  }
  return utf16;
}
#endif

#if defined(_WIN32)
std::string Term::Private::wide_to_utf8(const std::wstring& utf16)
{
  if(utf16.empty()) return std::string{};
  static constexpr DWORD kFlags{WC_ERR_INVALID_CHARS};
  if(utf16.length() > static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::overflow_error("Input string too long: size_t-length doesn't fit into int.");
  const int utf8Length{::WideCharToMultiByte(CP_UTF8, kFlags, utf16.data(), static_cast<int>(utf16.length()), nullptr, 0, nullptr, nullptr)};
  if(utf8Length == 0)
  {
    const DWORD error{::GetLastError()};
    throw Term::Exception(error, error == ERROR_NO_UNICODE_TRANSLATION ? "Invalid UTF-16 sequence found in input string." : "Cannot get result string length when converting from UTF-16 to UTF-8 (WideCharToMultiByte failed).");
  }
  std::string utf8;
  utf8.resize(utf8Length);
  int result{::WideCharToMultiByte(CP_UTF8, kFlags, utf16.data(), static_cast<int>(utf16.length()), &utf8[0], utf8Length, nullptr, nullptr)};
  if(result == 0)
  {
    const DWORD error{::GetLastError()};
    throw Term::Exception(error, error == ERROR_NO_UNICODE_TRANSLATION ? "Invalid UTF-16 sequence found in input string." : "Cannot convert from UTF-16 to UTF-8 (WideCharToMultiByte failed).");
  }
  return utf8;
}
#endif