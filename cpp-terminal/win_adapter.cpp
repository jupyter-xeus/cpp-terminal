/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2026 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/win_adapter.hpp"

#include "cpp-terminal/private/vt_parser.hpp"
#include "cpp-terminal/terminfo.hpp"

#include <iostream>

namespace Term
{
struct WinConsoleAdapter::impl
{
  impl() : emulator(Private::get_emulator()) {}
  ~impl()                          = default;
  impl(const impl& rhs)            = delete;
  impl& operator=(const impl& rhs) = delete;
  impl(impl&& other)               = delete;
  impl& operator=(impl&& other)    = delete;

  std::shared_ptr<Private::VtEmulator> emulator;
};

WinConsoleAdapter::WinConsoleAdapter(std::ostream& stream, size_t buf_size, std::size_t history_len) : m_stream(stream), m_buf(buf_size), m_history_len(history_len)
{
  // If this stream is one of the standard lib's streams, do nothing if not a legacy windows device.
  bool std_stream = stream.rdbuf() == std::cout.rdbuf() || stream.rdbuf() == std::cerr.rdbuf() || stream.rdbuf() == std::clog.rdbuf();
  bool legacy_win = Term::Terminfo::get(Term::Terminfo::Bool::Legacy);
  if(std_stream && !legacy_win) return;

  m_pimpl = std::unique_ptr<impl>(new impl());

  m_streambuf = stream.rdbuf();
  m_stream.rdbuf(this);

  char* beg = m_buf.data();
  setg(beg, beg, beg);
  setp(beg, beg + m_buf.capacity());
}

WinConsoleAdapter::~WinConsoleAdapter()
{
  if(m_streambuf) { m_stream.rdbuf(m_streambuf); }
}

WinConsoleAdapter::int_type WinConsoleAdapter::overflow(int c)
{
  sync();

  auto g_beg = eback();
  auto g_cur = gptr();
  auto g_end = egptr();
  auto p_cur = pptr();
  auto p_end = epptr();

  // Check if we can reclaim some space.
  auto read = g_cur - g_beg;
  if(read <= static_cast<std::streambuf::off_type>(m_history_len)) return traits_type::eof();

  using it_diff_type = decltype(m_buf)::iterator::difference_type;

  auto reclaim = read - m_history_len;
  std::move(m_buf.begin() + static_cast<it_diff_type>(reclaim), m_buf.end(), m_buf.begin());
  p_cur -= reclaim;
  g_cur -= reclaim;
  g_end -= reclaim;

  setg(g_beg, g_cur, g_end);
  setp(p_cur, p_end);

  if(traits_type::eq_int_type(c, traits_type::eof())) { return traits_type::not_eof(c); }

  sputc(traits_type::to_char_type(c));

  return c;
}

WinConsoleAdapter::int_type WinConsoleAdapter::underflow()
{
  auto g_beg = eback();
  auto g_cur = gptr();
  auto g_end = egptr();
  auto p_cur = pptr();

  if(g_end >= p_cur) { return traits_type::eof(); }

  setg(g_beg, g_cur, p_cur);

  return traits_type::to_int_type(*g_cur);
}

int WinConsoleAdapter::sync()
{
  try
  {
    Private::parse(this, [this](Private::VtSequence seq) { m_pimpl->emulator->process(seq); });
  }
  catch(const std::exception& e)
  {
    m_pimpl->emulator->report_error(e.what());
  }

  return 0;
}

WinConsoleAdapter::pos_type WinConsoleAdapter::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
  // This method is being overridden solely to support istream::tellg().
  if(off != 0) return -1;
  (void)dir;
  (void)which;
  return gptr() - eback();
}
}  // namespace Term
