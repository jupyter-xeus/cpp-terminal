/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2024 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/private/sigwinch.hpp"

#include "cpp-terminal/private/exception.hpp"

#if !defined(_WIN32)
  #include <csignal>
  #include <unistd.h>
#endif

#if defined(__linux__)
  #include <sys/signalfd.h>
#endif

#if defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
namespace Term
{
namespace Private
{
volatile std::sig_atomic_t m_signalStatus{0};
static void                sigwinchHandler(int sig)
{
  if(sig == SIGWINCH) { m_signalStatus = 1; }
  else { m_signalStatus = 0; }
}
}  // namespace Private
}  // namespace Term
#endif

std::int32_t Term::Private::Sigwinch::get() noexcept
{
#if defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
  return Term::Private::m_signalStatus;
#else
  return m_fd;
#endif
}

std::int32_t Term::Private::Sigwinch::m_fd{-1};

void Term::Private::Sigwinch::registerSigwinch()
{
#if defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
  struct sigaction sa;
  Term::Private::Errno().check_if(sigemptyset(&sa.sa_mask) != 0).throw_exception("sigemptyset(&sa.sa_mask)");
  sa.sa_flags   = {0};
  sa.sa_handler = {Term::Private::sigwinchHandler};
  Term::Private::Errno().check_if(sigaction(SIGWINCH, &sa, nullptr) != 0).throw_exception("sigaction(SIGWINCH, &sa, nullptr)");
#elif defined(__linux__)
  ::sigset_t windows_event = {};
  Term::Private::Errno().check_if(sigemptyset(&windows_event) != 0).throw_exception("sigemptyset(&windows_event)");
  Term::Private::Errno().check_if(sigaddset(&windows_event, SIGWINCH) != 0).throw_exception("sigaddset(&windows_event, SIGWINCH)");
  Term::Private::Errno().check_if((m_fd = ::signalfd(-1, &windows_event, SFD_NONBLOCK | SFD_CLOEXEC)) == -1).throw_exception("m_fd = ::signalfd(-1, &windows_event, SFD_NONBLOCK | SFD_CLOEXEC)");
#endif
}

void Term::Private::Sigwinch::blockSigwinch()
{
#if !defined(_WIN32)
  ::sigset_t windows_event = {};
  Term::Private::Errno().check_if(sigemptyset(&windows_event) != 0).throw_exception("sigemptyset(&windows_event)");
  Term::Private::Errno().check_if(sigaddset(&windows_event, SIGWINCH) != 0).throw_exception("sigaddset(&windows_event, SIGWINCH)");
  Term::Private::Errno().check_if(::pthread_sigmask(SIG_BLOCK, &windows_event, nullptr) != 0).throw_exception("::pthread_sigmask(SIG_BLOCK, &windows_event, nullptr)");
#endif
}

void Term::Private::Sigwinch::unblockSigwinch()
{
#if !defined(_WIN32)
  ::sigset_t windows_event = {};
  Term::Private::Errno().check_if(sigemptyset(&windows_event) != 0).throw_exception("sigemptyset(&windows_event)");
  Term::Private::Errno().check_if(sigaddset(&windows_event, SIGWINCH) != 0).throw_exception("sigaddset(&windows_event, SIGWINCH)");
  Term::Private::Errno().check_if(::pthread_sigmask(SIG_UNBLOCK, &windows_event, nullptr) != 0).throw_exception("::pthread_sigmask(SIG_UNBLOCK, &windows_event, nullptr)");
#endif
}

bool Term::Private::Sigwinch::isSigwinch(const std::int32_t& file_descriptor) noexcept
{
#if defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
  if(Term::Private::m_signalStatus == 1)
  {
    static_cast<void>(file_descriptor);  // suppress warning
    Term::Private::m_signalStatus = {0};
    return true;
  }
  return false;
#elif defined(__linux__)
  if(m_fd == file_descriptor)
  {
    // read it to clean
    ::signalfd_siginfo fdsi = {};
    ::read(m_fd, &fdsi, sizeof(fdsi));
    return true;
  }
  return false;
#else
  static_cast<void>(file_descriptor);  // suppress warning
  return false;
#endif
}
