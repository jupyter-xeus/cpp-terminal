/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2025 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/private/signals.hpp"

#include "cpp-terminal/terminal.hpp"

#include <algorithm>
#include <csignal>

#ifndef NSIG
  #define NSIG (_SIGMAX + 1) /* For QNX */
#endif

const std::size_t Term::Private::Signals::m_signals_number{NSIG - 1};

void Term::Private::Signals::setHandler(const sighandler_t& handler) noexcept
{
  for(std::size_t signal = 0; signal != m_signals_number; ++signal) { sighandler_t hand = std::signal(signal, handler); }
}

Term::Private::Signals::Signals(std::vector<sighandler_t>& m_han) noexcept
{
  const static std::vector<int> ignore{
#if defined(SIGCONT)
    SIGCONT,
#endif
#if defined(SIGSTOP)
    SIGSTOP,
#endif
#if defined(SIGTSTP)
    SIGTSTP,
#endif
#if defined(SIGTTIN)
    SIGTTIN,
#endif
#if defined(SIGTTOU)
    SIGTTOU,
#endif
  };
  m_han.reserve(m_signals_number);
  for(std::size_t signal = 0; signal != m_signals_number; ++signal)
  {
    //if(std::find(ignore.begin(),ignore.end(),signal)==ignore.end())
    //{
    sighandler_t old = std::signal(signal, SIG_DFL);
    //sighandler_t dumb=std::signal(signal, old);
    m_han.push_back(old);
    //}
    //else
    //{
    //  std::signal(signal, SIG_IGN);
    //  m_han.push_back(std::signal(signal, SIG_IGN));
    // }
  }
}

void Term::Private::Signals::reset_and_raise(int sign, std::vector<sighandler_t>& m_han, Term::Terminal& term) noexcept
{
  const static std::vector<int> termin{
#if defined(SIGHUP)
    SIGHUP,
#endif
#if defined(SIGHUP)
    SIGINT,
#endif
#if defined(SIGQUIT)
    SIGQUIT,
#endif
#if defined(SIGQUIT)
    SIGILL,
#endif
#if defined(SIGTRAP)
    SIGTRAP,
#endif
#if defined(SIGTRAP)
    SIGABRT,
#endif
#if defined(SIGIOT)
    SIGIOT,
#endif
#if defined(SIGBUS)
    SIGBUS,
#endif
#if defined(SIGBUS)
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
#if defined(SIGUSR2)
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
#if defined(SIGPROF)
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
    sighandler_t old = std::signal(sign, m_han[sign]);
    old              = std::signal(sign, m_han[sign]);
    term.clean();
    std::raise(sign);
  }
}
