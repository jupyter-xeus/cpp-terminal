/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/platforms/sigwinch.hpp"

#if !defined(_WIN32)
  #include <csignal>
  #include <unistd.h>
#endif

#if defined(__linux__)
  #include <sys/signalfd.h>
#endif

namespace Term
{
namespace Private
{
#if defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
volatile std::sig_atomic_t m_signalStatus{0};
static void                sigwinchHandler(int sig)
{
  if(sig == SIGWINCH) m_signalStatus = 1;
  else
    m_signalStatus = 0;
}
#endif
}  // namespace Private
}  // namespace Term

int Term::Private::Sigwinch::get()
{
#if defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
  return Term::Private::m_signalStatus;
#else
  return m_fd;
#endif
}

int Term::Private::Sigwinch::m_fd{-1};

void Term::Private::Sigwinch::registerSigwinch()
{
#if defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags   = {0};
  sa.sa_handler = {Term::Private::sigwinchHandler};
  sigaction(SIGWINCH, &sa, nullptr);
#elif defined(__linux__)
  ::sigset_t windows_event;
  sigemptyset(&windows_event);
  sigaddset(&windows_event, SIGWINCH);
  m_fd = {::signalfd(-1, &windows_event, SFD_NONBLOCK | SFD_CLOEXEC)};
#endif
}

void Term::Private::Sigwinch::blockSigwinch()
{
#if !defined(_WIN32)
  ::sigset_t windows_event;
  sigemptyset(&windows_event);
  sigaddset(&windows_event, SIGWINCH);
  ::pthread_sigmask(SIG_BLOCK, &windows_event, nullptr);
#endif
  registerSigwinch();
}

void Term::Private::Sigwinch::unblockSigwinch()
{
#if !defined(_WIN32)
  ::sigset_t windows_event;
  sigemptyset(&windows_event);
  sigaddset(&windows_event, SIGWINCH);
  ::pthread_sigmask(SIG_UNBLOCK, &windows_event, nullptr);
#endif
}

bool Term::Private::Sigwinch::isSigwinch(const int& fd)
{
#if defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
  if(Term::Private::m_signalStatus == 1)
  {
    static_cast<void>(fd);  // suppress warning
    Term::Private::m_signalStatus = {0};
    return true;
  }
  else
    return false;
#elif defined(__linux__)
  if(m_fd == fd)
  {
    // read it to clean
    ::signalfd_siginfo fdsi;
    ::read(m_fd, &fdsi, sizeof(fdsi));
    return true;
  }
  else
    return false;
#else
  static_cast<void>(fd);  // suppress warning
  return false;
#endif
}
