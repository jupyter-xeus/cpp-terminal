/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/platforms/file_initializer.hpp"

#include "cpp-terminal/platforms/exception.hpp"
#include "cpp-terminal/platforms/file.hpp"
#include "cpp-terminal/platforms/return_code.hpp"

#include <iostream>
#include <ostream>

#if defined(_WIN32)
  #include <windows.h>
  #include <io.h>
  #include "cpp-terminal/platforms/unicode.hpp"
#else
  #include <sys/stat.h>
#endif

void Term::Private::FileInitializer::init()
{
  try
  {
    // MacOS was not happy wish a static mutex in the class so we create it and pass to each class;
    static std::recursive_mutex ioMutex;
    if(0 == m_counter)
    {
      attachConsole();
      if(new(&Term::Private::in) InputFileHandler(ioMutex) == nullptr) { throw Term::Exception("new(&Term::Private::in) InputFileHandler(ioMutex)"); }
      if(new(&Term::Private::out) OutputFileHandler(ioMutex) == nullptr) { throw Term::Exception("new(&Term::Private::out) OutputFileHandler(ioMutex)"); }
    }
    ++m_counter;
  }
  catch(const Term::Exception& exception)
  {
    std::cerr << "Error in cpp-terminal: " << exception.what() << '\n' << std::flush;
    std::exit(Term::returnCode());
  }
  catch(...)
  {
    std::cerr << "Undefined error in cpp-terminal [Term::Private::FileInitializer::init()]" << '\n' << std::flush;
    std::exit(Term::returnCode());
  }
}

bool Term::Private::FileInitializer::m_consoleCreated = {false};

std::size_t Term::Private::FileInitializer::m_counter = {0};

///
///@brief Attach the console
///
/// Check if a console is attached to the process. If not, try to attach to the console. If there is no console, then create one. \b stdin, \b stdout, \b stderr are check and opened if necessary.
/// On error, on window, a message box is raised.
///
void Term::Private::FileInitializer::attachConsole()
try
{
#if defined(_WIN32)
  // If something happen here we still don't have a console so we can only use a MessageBox to warn the users something is very bad and that they should contact us.
  Term::Private::WindowsError error{Term::Private::WindowsError().check_if(AttachConsole(ATTACH_PARENT_PROCESS) == 0)};
  bool                        need_allocation{false};
  switch(error.error())
  {
    case ERROR_SUCCESS: break;                                                                      // Has been attached
    case ERROR_ACCESS_DENIED: need_allocation = false; break;                                       // Already attached that's good !
    case ERROR_INVALID_PARAMETER: error.throw_exception("The specified process does not exist !");  // Should never happen !
    case ERROR_INVALID_HANDLE: need_allocation = true; break;                                       // Need to allocate th console !
  }
  if(need_allocation)
  {
    Term::Private::WindowsError().check_if(AllocConsole() == 0).throw_exception("AllocConsole()");
    m_consoleCreated = true;
  }
  FILE* fDummy{nullptr};
  if(_fileno(stdout) < 0 || _get_osfhandle(_fileno(stdout)) < 0) Term::Private::WindowsError().check_if(freopen_s(&fDummy, "CONOUT$", "w", stdout) != 0).throw_exception("freopen_s(&fDummy, \"CONOUT$\", \"w\", stdout)");
  if(_fileno(stderr) < 0 || _get_osfhandle(_fileno(stderr)) < 0) Term::Private::WindowsError().check_if(freopen_s(&fDummy, "CONOUT$", "w", stderr) != 0).throw_exception("freopen_s(&fDummy, \"CONOUT$\", \"w\", stderr)");
  if(_fileno(stdin) < 0 || _get_osfhandle(_fileno(stdin)) < 0) Term::Private::WindowsError().check_if(freopen_s(&fDummy, "CONIN$", "r", stdin) != 0).throw_exception("freopen_s(&fDummy, \"CONIN$\", \"r\", stdin)");
  const std::size_t bestSize{BUFSIZ > 4096 ? BUFSIZ : 4096};
#else
  const struct stat stats
  {
  };
  const std::size_t bestSize{static_cast<size_t>(stats.st_blksize)};
#endif
  Term::Private::Errno().check_if(std::setvbuf(stdin, nullptr, _IOLBF, bestSize) != 0).throw_exception("std::setvbuf(stdin, nullptr, _IOLBF, bestSize)");
  Term::Private::Errno().check_if(std::setvbuf(stdout, nullptr, _IOLBF, bestSize) != 0).throw_exception("std::setvbuf(stdout, nullptr, _IOLBF, bestSize)");
  Term::Private::Errno().check_if(std::setvbuf(stderr, nullptr, _IOLBF, bestSize) != 0).throw_exception("std::setvbuf(stderr, nullptr, _IOLBF, bestSize)");
}
catch(const Term::Exception& exception)
{
  detachConsole();
#if defined(_WIN32)
  MessageBoxW(nullptr, Term::Private::to_wide(exception.what()).c_str(), Term::Private::to_wide("cpp-terminal").c_str(), MB_OK | MB_ICONERROR | MB_DEFBUTTON1);
  std::exit(exception.code());
#endif
  throw;
}

void Term::Private::FileInitializer::detachConsole()
{
#if defined(_WIN32)
  if(m_consoleCreated) FreeConsole();
#endif
}

Term::Private::FileInitializer::FileInitializer() { init(); }

Term::Private::FileInitializer::~FileInitializer()
{
  if(0 == m_counter)
  {
    (&Term::Private::in)->~InputFileHandler();
    (&Term::Private::out)->~OutputFileHandler();
    detachConsole();
  }
  --m_counter;
}
