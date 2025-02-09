/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/private/signals.hpp"

#include "cpp-terminal/terminal_impl.hpp"

#include <algorithm>
#include <csignal>

#ifndef NSIG
  #define NSIG (_SIGMAX + 1) /* For QNX */
#endif

#ifdef _WIN32
  #pragma warning(push)
  #pragma warning(disable : 4668)
  #include <windows.h>
  #pragma warning(pop)
static BOOL WINAPI consoleHandler(DWORD signal)
{
  switch(signal)
  {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    {
      Term::Private::Signals::clean_terminal();
      return false;
    }
    default: return false;
  }
}
#endif

void Term::Private::Signals::clean_terminal() noexcept { const_cast<Term::Terminal*>(m_term)->clean(); }

const std::size_t Term::Private::Signals::m_signals_number{NSIG - 1};

void Term::Private::Signals::setHandler(const sighandler_t& handler) noexcept
{
  for(std::size_t signal = 0; signal != m_signals_number; ++signal)
  {
#ifdef _WIN32
    if(signal != SIGINT) sighandler_t hand = std::signal(signal, handler);
#else
    sighandler_t hand = std::signal(signal, handler);
#endif
  }
#ifdef _WIN32
  SetConsoleCtrlHandler(consoleHandler, TRUE);
#endif
}

const Term::Terminal* Term::Private::Signals::m_term{nullptr};

Term::Private::Signals::Signals(const Terminal& terminal) noexcept
{
  m_term = &terminal;
  m_handlers.reserve(m_signals_number);
  for(std::size_t signal = 0; signal != m_signals_number; ++signal)
  {
    //if(std::find(ignore.begin(),ignore.end(),signal)==ignore.end())
    //{
    sighandler_t old = std::signal(signal, SIG_DFL);
    //sighandler_t dumb=std::signal(signal, old);
    m_handlers.push_back(old);
    //}
    //else
    //{
    //  std::signal(signal, SIG_IGN);
    //  m_han.push_back(std::signal(signal, SIG_IGN));
    // }
  }
}

void Term::Private::Signals::reset_and_raise(const int& sign) noexcept
{
  clean_terminal();
  const static std::vector<int> termin{
#if defined(SIGHUP)
    SIGHUP,
#endif
#if defined(SIGINT)
    SIGINT,
#endif
#if defined(SIGQUIT)
    SIGQUIT,
#endif
#if defined(SIGILL)
    SIGILL,
#endif
#if defined(SIGTRAP)
    SIGTRAP,
#endif
#if defined(SIGABRT)
    SIGABRT,
#endif
#if defined(SIGIOT)
    SIGIOT,
#endif
#if defined(SIGBUS)
    SIGBUS,
#endif
#if defined(SIGFPE)
    SIGFPE,
#endif
#if defined(SIGKILL)
    SIGKILL,
#endif
#if defined(SIGUSR1)
    SIGUSR1,
#endif
#if defined(SIGSEGV)
    SIGSEGV,
#endif
#if defined(SIGUSR2)
    SIGUSR2,
#endif
#if defined(SIGPIPE)
    SIGPIPE,
#endif
#if defined(SIGALRM)
    SIGALRM,
#endif
#if defined(SIGSTKFLT)
    SIGSTKFLT,
#endif
#if defined(SIGCONT)
    SIGCONT,
#endif
#if defined(SIGXCPU)
    SIGXCPU,
#endif
#if defined(SIGXFSZ)
    SIGXFSZ,
#endif
#if defined(SIGVTALRM)
    SIGVTALRM,
#endif
#if defined(SIGPROF)
    SIGPROF,
#endif
#if defined(SIGIO)
    SIGIO,
#endif
#if defined(SIGPOLL)
    SIGPOLL,
#endif
#if defined(SIGPWR)
    SIGPWR,
#endif
#if defined(SIGSYS)
    SIGSYS,
#endif
#if defined(SIGUNUSED)
    SIGUNUSED,
#endif
#if defined(SIGUNUSED)
    SIGUNUSED,
#endif
#if defined(SIGTERM)
    SIGTERM
#endif
  };
  if(std::find(termin.begin(), termin.end(), sign) != termin.end())
  {
    sighandler_t old = std::signal(sign, m_handlers[sign]);
    old              = std::signal(sign, m_handlers[sign]);
    std::raise(sign);
  }
}
