/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2026 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once
#include <functional>
#include <memory>
#include <ostream>
#include <streambuf>
#include <vector>

namespace Term
{
/**
	 * @brief Emulates the virtual terminal features Microsoft introduced in 
	 *			Windows 10 for pre-win10 systems starting with Windows 2000.
	 */
class WinConsoleAdapter : public std::streambuf
{
public:
  using int_type = std::streambuf::traits_type::int_type;
  using pos_type = std::streambuf::pos_type;

  WinConsoleAdapter(std::ostream& stream, size_t buf_size = 1024, std::size_t min_history = 128);
  ~WinConsoleAdapter();
  WinConsoleAdapter(const WinConsoleAdapter& rhs)            = delete;
  WinConsoleAdapter& operator=(const WinConsoleAdapter& rhs) = delete;
  WinConsoleAdapter(WinConsoleAdapter&& other)               = delete;
  WinConsoleAdapter& operator=(WinConsoleAdapter&& other)    = delete;

protected:
  virtual int_type overflow(int c) override;
  virtual int_type underflow() override;
  virtual int      sync() override;
  virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override;

private:
  struct impl;
  std::unique_ptr<impl> m_pimpl;

  std::ostream&                    m_stream;
  std::streambuf*                  m_streambuf{nullptr};
  std::vector<char>                m_buf;
  std::size_t                      m_history_len;
};

}  // namespace Term
