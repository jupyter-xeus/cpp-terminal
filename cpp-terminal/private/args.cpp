/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/args.hpp"

#include <ios>

#if defined(_WIN32)
  #include <memory>
// clang-format off
  #include <windows.h>
  #include <processenv.h>
  #include "cpp-terminal/private/unicode.hpp"
// clang-format on
#elif defined(__APPLE__)
  #include <crt_externs.h>
#else
  #include <algorithm>
  #include <cstddef>
  #include <fstream>
  #include <limits>
#endif

void Term::Arguments::parse()
{
  if(m_parsed) { return; }
#if defined(_WIN32)
  int                                            argc{0};
  std::unique_ptr<LPWSTR[], void (*)(wchar_t**)> wargv = std::unique_ptr<LPWSTR[], void (*)(wchar_t**)>(CommandLineToArgvW(GetCommandLineW(), &argc), [](wchar_t** ptr) { LocalFree(ptr); });
  if(wargv == nullptr)
  {
    m_parsed = false;
    return;
  }
  else
  {
    m_args.reserve(static_cast<std::size_t>(argc));
    for(std::size_t i = 0; i != static_cast<std::size_t>(argc); ++i) { m_args.push_back(Term::Private::to_narrow(&wargv.get()[i][0])); }
    m_parsed = true;
  }
#elif defined(__APPLE__)
  std::size_t argc{static_cast<std::size_t>(*_NSGetArgc())};
  m_args.reserve(argc);
  char** argv{*_NSGetArgv()};
  for(std::size_t i = 0; i != argc; ++i) { m_args.push_back(argv[i]); }
  m_parsed = true;
#else
  std::string                 cmdline;
  std::fstream                file_stream;
  const std::fstream::iostate old_iostate{file_stream.exceptions()};
  try
  {
    file_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file_stream.open("/proc/self/cmdline", std::fstream::in | std::fstream::binary);
    file_stream.ignore(std::numeric_limits<std::streamsize>::max());
    cmdline.resize(static_cast<std::size_t>(file_stream.gcount()));
    file_stream.seekg(0, std::ios_base::beg);
    file_stream.get(&cmdline[0], static_cast<std::streamsize>(cmdline.size()));  //NOLINT(readability-container-data-pointer)
    file_stream.exceptions(old_iostate);
    if(file_stream.is_open()) { file_stream.close(); }
    m_args.reserve(static_cast<std::size_t>(std::count(cmdline.begin(), cmdline.end(), '\0')));
    for(std::string::iterator it = cmdline.begin(); it != cmdline.end(); it = std::find(it, cmdline.end(), '\0') + 1) { m_args.emplace_back(cmdline.data() + (it - cmdline.begin())); }
    m_parsed = true;
  }
  catch(...)
  {
    file_stream.exceptions(old_iostate);
    if(file_stream.is_open()) { file_stream.close(); }
    m_args.clear();
    m_parsed = false;
  }
#endif
}

std::size_t Term::Arguments::argc()
{
  parse();
  return m_args.size();
}

std::vector<std::string> Term::Arguments::argv()
{
  parse();
  return m_args;
}

bool Term::Arguments::m_parsed = false;

std::vector<std::string> Term::Arguments::m_args = std::vector<std::string>();  //NOLINT(fuchsia-statically-constructed-objects)
