#include "cpp-terminal/args.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>

void Term::Arguments::parse()
{
  if(m_parsed == true) return;
#if defined(_WIN32)
  #elseif defined(__APPLE__)
#else
  std::string           cmdline;
  std::fstream          fs;
  std::fstream::iostate old_iostate{fs.exceptions()};
  try
  {
    fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fs.open("/proc/self/cmdline", std::fstream::in | std::fstream::binary);
    fs.ignore(std::numeric_limits<std::streamsize>::max());
    cmdline.resize(fs.gcount());
    fs.seekg(0, std::ios_base::beg);
    fs.get(&cmdline[0], cmdline.size());
    fs.exceptions(old_iostate);
    if(fs.is_open()) fs.close();
    const std::size_t argc = static_cast<std::size_t>(std::count(cmdline.begin(), cmdline.end(), '\0'));
    m_args.reserve(argc);
    for(std::string::iterator it = cmdline.begin(); it != cmdline.end(); it = std::find(it, cmdline.end(), '\0') + 1) { m_args.push_back(cmdline.data() + (it - cmdline.begin())); }
    m_parsed = true;
  }
  catch(...)
  {
    fs.exceptions(old_iostate);
    if(fs.is_open()) fs.close();
    m_parsed = false;
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

std::vector<std::string> Term::Arguments::m_args = std::vector<std::string>();
