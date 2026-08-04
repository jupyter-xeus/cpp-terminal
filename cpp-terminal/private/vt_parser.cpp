/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2026 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

#include "cpp-terminal/color.hpp"
#include "cpp-terminal/private/vt_parser.hpp"

#include <algorithm>
#include <istream>
#include <iterator>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <wchar.h>

/* References

Windows console functions and associatedd ANSI escape sequences.
https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

Wikipedia article
https://en.wikipedia.org/wiki/ANSI_escape_code

Paul Flo Williams's state diagram
https://vt100.net/emu/dec_ansi_parser

ANSI Escape Sequences gist
https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

// ANSICON
https://github.com/adoxa/ansicon
*/

namespace
{
using namespace Term::Private;

// Single emulator to be used by all writers/adapters.
std::shared_ptr<VtEmulator> emulator;

enum Char : char
{
  NUL          = '\0',
  BEL          = '\x7',
  CAN          = '\x18',  // Cancel
  SUB          = '\x1a',  // Substitute
  ESC          = '\x1b',
  Colon        = '\x3a',
  Semicolon    = '\x3b',
  LessThan     = '\x3c',
  Equals       = '\x3d',
  GreaterThan  = '\x3e',
  QuestionMark = '\x3f',
  CSI          = '\x5b',  // Control Sequence Introducer 'ESC ['
  Backslash    = '\x5c',
  OSC_7bit     = '\x5d',  // Operating System Command ('ESC ]' for 7-bit systems but widely used in 8-bit as well)
  DEL          = '\x7f',
  SOS          = '\x98',  // Start of string
  OSC_8bit     = '\x9d',  // Operating System Command (used in 8-bit systems)
  ST           = '\x9c',  // String Terminator
  PM           = '\x9e',  // Private Message
  APC          = '\x9f',  // Application Program Command
};

// C0 control codes, except for a few (0x18 Cancel, 0x1a Substitute 0x1b Escape)
bool is_c0(char c) { return (c >= 0x00 && c <= 0x17) || c == 0x19 || (c >= 0x1c && c <= 0x1f); }

// Intermediate codes 0x20 to 0x2F
bool is_intermediate(char c) { return c >= 0x20 && c <= 0x2F; }

// CSI parameters 0x30 to 0x39 && 0x3b
bool is_csi_param(char c) { return (c >= 0x30 && c <= 0x39) || c == 0x3b; }

// CSI "private" mode indicators (manufacturer-specific) 0x3c to 0x3f
bool is_csi_private(char c) { return c >= 0x3c && c <= 0x3f; }

void throw_unhandled_char(char c) { throw std::runtime_error("Unhandled character '" + std::to_string(c) + "'"); }

int utf8_leading_ones(char c)
{
  int ones{0};
  int mask{0x80};

  while((c & mask) == mask)
  {
    ++ones;
    mask = mask >> 1;
  }

  return ones;
};

/**
   * @brief State machine for parsing video terminal control sequences, based on the state digram by Paul Flo Williams
   *      https://vt100.net/emu/dec_ansi_parser
   */
class VtParseState;
class VtParseMachine
{
public:
  VtParseMachine()  = delete;
  ~VtParseMachine() = default;
  VtParseMachine(std::streambuf* sb, std::function<void(VtSequence)> cb);
  VtParseMachine(const VtParseMachine& rhs)            = delete;
  VtParseMachine& operator=(const VtParseMachine& rhs) = delete;
  VtParseMachine(VtParseMachine&& other)               = delete;
  VtParseMachine& operator=(VtParseMachine&& other)    = delete;

  void run();

  // Emits what has been collected with the specified type
  void emit(VtSequence::Type type);

  void emit(char c, VtSequence::Type type);

  void change_state(std::unique_ptr<VtParseState>&& new_handler);

  bool pending_transition() const noexcept;

  void take();

  void ignore(std::streamsize n = 1);

  void clear_collected();

  const std::string& collected() const noexcept;

private:
  void perform_transition();

  std::unique_ptr<VtParseState>   m_handler;
  std::unique_ptr<VtParseState>   m_pending_handler;
  std::istream                    m_stream;
  std::string                     m_collected;
  std::function<void(VtSequence)> m_emit_cb;
};

class VtParseState
{
public:
  virtual ~VtParseState() = default;

  virtual void enter(VtParseMachine& m) { (void)m; }
  virtual void exit(VtParseMachine& m) { (void)m; }
  virtual void handle(char c, VtParseMachine& m) = 0;

  bool handle_anywhere_control_codes(char c, VtParseMachine& m);
};

/**
 * @brief This is NOT a handler state. It's an extension of the base
 *        class to provide support for UTF-8 character handling to handler
 *        classes that need it.
 *
 *        A UTF-8 handler class is tempting, but would require jumping
 *        out of ground state for each UTF-8 char, which is less than ideal.
 *        Not repeating the handling code is good enough.
 */
class VtParseStateUtf8 : public VtParseState
{
protected:
  ~VtParseStateUtf8() = default;

  bool handle_utf8(char c, VtParseMachine& m);

  int utf8_bytes_remaining{};
};

class VtParseStateGround : public VtParseStateUtf8
{
public:
  void handle(char c, VtParseMachine& m) override;
  void exit(VtParseMachine& m) override;
};

class VtParseStateEscape : public VtParseState
{
public:
  void enter(VtParseMachine& m) override;
  void handle(char c, VtParseMachine& m) override;
};

class VtParseStateEscapeIntermediate : public VtParseState
{
public:
  void handle(char c, VtParseMachine& m) override;
};

class VtParseStateCsiEntry : public VtParseState
{
public:
  void enter(VtParseMachine& m) override;
  void handle(char c, VtParseMachine& m) override;
};

class VtParseStateCsiParam : public VtParseState
{
public:
  void handle(char c, VtParseMachine& m) override;
};

class VtParseStateCsiIgnore : public VtParseState
{
public:
  void handle(char c, VtParseMachine& m) override;
};

class VtParseStateCsiIntermediate : public VtParseState
{
public:
  void handle(char c, VtParseMachine& m) override;
};

class VtParseStateOsc : public VtParseStateUtf8
{
public:
  void enter(VtParseMachine& m) override;
  void handle(char c, VtParseMachine& m) override;
  void exit(VtParseMachine& m) override;
};

class VtParseStateSos : public VtParseState
{
public:
  void handle(char c, VtParseMachine& m) override;
};

VtParseMachine::VtParseMachine(std::streambuf* sb, std::function<void(VtSequence)> cb) : m_handler(std::unique_ptr<VtParseStateGround>(new VtParseStateGround())), m_stream(sb), m_emit_cb(cb) {}

void VtParseMachine::run()
{
  std::ostream::pos_type g_pos{};
  int                    g_pos_unchanged{0};

  while(m_stream.peek() != std::ostream::traits_type::eof())
  {
    auto c = static_cast<char>(m_stream.peek());
    m_handler->handle(c, *this);

    auto pos = m_stream.tellg();
    if(pos == g_pos)
    {
      if(++g_pos_unchanged > 3) { throw std::runtime_error("Infinite loop detected"); }
    }
    else
    {
      g_pos           = pos;
      g_pos_unchanged = 0;
    }

    if(pending_transition()) perform_transition();
  }

  m_handler->exit(*this);

  while(!m_collected.empty() && m_stream)
  {
    if(m_stream.unget()) m_collected.pop_back();
  }
}

void VtParseMachine::take() { m_collected += static_cast<char>(m_stream.get()); }

void VtParseMachine::ignore(std::streamsize n) { m_stream.ignore(n); }

void VtParseMachine::clear_collected() { m_collected.clear(); }

const std::string& VtParseMachine::collected() const noexcept { return m_collected; }

void VtParseMachine::perform_transition()
{
  if(m_pending_handler)
  {
    m_handler->exit(*this);
    m_handler = std::move(m_pending_handler);
    m_handler->enter(*this);
  }
}

void VtParseMachine::emit(VtSequence::Type type)
{
  if(m_collected.empty()) return;

  try
  {
    m_emit_cb(VtSequence(type, m_collected));
  }
  catch(const std::runtime_error& exp)
  {
    m_emit_cb(VtSequence(VtSequence::Type::Malformed, exp.what()));
  }

  m_collected.clear();
}

void VtParseMachine::emit(char c, VtSequence::Type type) { VtSequence s(type, std::string(1, c)); }

void VtParseMachine::change_state(std::unique_ptr<VtParseState>&& new_handler) { m_pending_handler = std::move(new_handler); }

bool VtParseMachine::pending_transition() const noexcept { return m_pending_handler ? true : false; }

bool VtParseState::handle_anywhere_control_codes(char c, VtParseMachine& m)
{
  bool handled{false};

  auto uc = static_cast<unsigned char>(c);

  if(c == Char::CAN || c == Char::SUB)
  {
    m.ignore();
    m.emit(c, VtSequence::Type::C0);
    m.change_state(std::unique_ptr<VtParseStateGround>(new VtParseStateGround()));
    handled = true;
  }
  else if((uc >= 0x80 && uc <= 0x8f) || (uc >= 0x91 && uc <= 0x97) || uc == 0x99 || uc == 0x9a)
  {
    m.ignore();
    m.emit(c, VtSequence::Type::C1);
    m.change_state(std::unique_ptr<VtParseStateGround>(new VtParseStateGround()));
    handled = true;
  }
  else if(c == Char::ST)
  {
    m.change_state(std::unique_ptr<VtParseStateGround>(new VtParseStateGround()));
    handled = true;
  }
  else if(c == Char::ESC)
  {
    m.change_state(std::unique_ptr<VtParseStateEscape>(new VtParseStateEscape()));
    handled = true;
  }
  else if(c == Char::SOS || c == Char::PM || c == Char::APC)
  {
    m.change_state(std::unique_ptr<VtParseStateSos>(new VtParseStateSos()));
    handled = true;
  }
  else if(c == Char::OSC_8bit)
  {
    m.change_state(std::unique_ptr<VtParseStateOsc>(new VtParseStateOsc()));
    handled = true;
  }

  return handled;
}

bool VtParseStateUtf8::handle_utf8(char c, VtParseMachine& m)
{
  auto ones = utf8_leading_ones(c);

  if(utf8_bytes_remaining)
  {
    if(ones == 1)
    {
      m.take();
      --utf8_bytes_remaining;
      return true;
    }

    // Expected UTF-8 continuation byte, but got something else. Abandon sequence and continue processing.
    utf8_bytes_remaining = 0;
  }
  else if(ones >= 2 && ones <= 4)
  {
    m.take();
    utf8_bytes_remaining = ones - 1;
    return true;
  }

  return false;
}

void VtParseStateGround::handle(char c, VtParseMachine& m)
{
  // Some UTF-8 continuation chars have some overlap with codes handled by handle_anywhere_control_codes(),
  // so consume UTF-8 first.
  if(handle_utf8(c, m)) return;

  if(handle_anywhere_control_codes(c, m)) return;

  if(is_c0(c))
  {
    m.emit(VtSequence::Type::Text);
    m.take();
    m.emit(VtSequence::Type::C0);
  }
  else if(c >= 0x20 && static_cast<unsigned char>(c) <= 0x7F) { m.take(); }
  else { throw_unhandled_char(c); }
}

void VtParseStateGround::exit(VtParseMachine& m)
{
  if(m.collected().empty()) return;

  // Incomplete escape sequence
  if(m.collected()[0] == Char::ESC) return;

  // Incomplete UTF-8 sequence
  if(utf8_bytes_remaining) return;

  m.emit(VtSequence::Type::Text);
}

void VtParseStateEscape::enter(VtParseMachine& m)
{
  // Clear invalid sequences that have been collected up to this point.
  if(!m.collected().empty()) { m.emit(VtSequence::Type::Malformed); }
}

void VtParseStateEscape::handle(char c, VtParseMachine& m)
{
  if(m.collected().empty())
  {
    if(c != Char::ESC) { throw std::runtime_error("The first character handled in VtParseStateEscape must be ESC"); }

    m.take();
    return;
  }

  if(handle_anywhere_control_codes(c, m)) return;

  if(is_c0(c))
  {
    m.ignore();
    m.emit(c, VtSequence::Type::C0);
  }
  else if(c == Char::DEL) { m.ignore(); }
  else if(is_intermediate(c))
  {
    m.take();
    m.change_state(std::unique_ptr<VtParseStateEscapeIntermediate>(new VtParseStateEscapeIntermediate()));
  }
  else if((c >= 0x30 && c <= 0x4f) || (c >= 0x51 && c <= 0x57) || c == 0x59 || c == 0x5a || c == 0x5c || (c >= 0x60 && c <= 0x7e))
  {
    m.take();
    m.emit(VtSequence::Type::Escape);
    m.change_state(std::unique_ptr<VtParseStateGround>(new VtParseStateGround()));
  }
  else if(c == 0x58 || c == 0x5e || c == 0x5f)
  {
    m.take();
    m.change_state(std::unique_ptr<VtParseStateSos>(new VtParseStateSos()));
  }
  else if(c == 0x50)
  {
    // Do DCS sequences need to be supported?
    throw_unhandled_char(c);
  }
  else if(c == 0x5d)
  {
    m.take();
    m.change_state(std::unique_ptr<VtParseStateOsc>(new VtParseStateOsc()));
  }
  else if(c == Char::CSI)
  {
    m.take();
    m.change_state(std::unique_ptr<VtParseStateCsiEntry>(new VtParseStateCsiEntry()));
  }
  else { throw_unhandled_char(c); }
}

void VtParseStateEscapeIntermediate::handle(char c, VtParseMachine& m)
{
  if(handle_anywhere_control_codes(c, m)) return;

  if(is_c0(c))
  {
    m.ignore();
    m.emit(c, VtSequence::Type::C0);
  }
  else if(c == Char::DEL) { m.ignore(); }
  else if(is_intermediate(c)) { m.take(); }
  else if(c >= 0x30 && c <= 0x7e)
  {
    m.take();
    m.emit(VtSequence::Type::Escape);
    m.change_state(std::unique_ptr<VtParseStateGround>(new VtParseStateGround()));
  }
  else { throw_unhandled_char(c); }
}

void VtParseStateCsiEntry::enter(VtParseMachine& m)
{
  // Clear invalid sequences that have been collected up to this point.
  if(m.collected().size() != 2 || m.collected()[0] != Char::ESC || m.collected()[1] != Char::CSI) { m.clear_collected(); }
}

void VtParseStateCsiEntry::handle(char c, VtParseMachine& m)
{
  if(handle_anywhere_control_codes(c, m)) return;

  if(is_c0(c))
  {
    m.ignore();
    m.emit(c, VtSequence::Type::C0);
  }
  else if(c == Char::DEL) { m.ignore(); }
  else if(is_csi_param(c))
  {
    m.take();
    m.change_state(std::unique_ptr<VtParseStateCsiParam>(new VtParseStateCsiParam()));
  }
  else if(is_csi_private(c))
  {
    m.take();
    m.change_state(std::unique_ptr<VtParseStateCsiParam>(new VtParseStateCsiParam()));
  }
  else if(c == 0x3a)
  {
    m.take();
    m.change_state(std::unique_ptr<VtParseStateCsiIgnore>(new VtParseStateCsiIgnore()));
  }
  else if(is_intermediate(c))
  {
    m.take();
    m.change_state(std::unique_ptr<VtParseStateCsiIntermediate>(new VtParseStateCsiIntermediate()));
  }
  else if(c >= 0x40 && c <= 0x7e)
  {
    m.take();
    m.emit(VtSequence::Type::CSI);
    m.change_state(std::unique_ptr<VtParseStateGround>(new VtParseStateGround()));
  }
  else { throw_unhandled_char(c); }
}

void VtParseStateCsiParam::handle(char c, VtParseMachine& m)
{
  if(handle_anywhere_control_codes(c, m)) return;

  if(is_c0(c))
  {
    m.ignore();
    m.emit(c, VtSequence::Type::C0);
  }
  else if(c == Char::DEL) { m.ignore(); }
  else if(is_csi_param(c)) { m.take(); }
  else if(c >= 0x40 && c <= 0x7e)
  {
    m.take();
    m.emit(VtSequence::Type::CSI);
    m.change_state(std::unique_ptr<VtParseStateGround>(new VtParseStateGround()));
  }
  else if(is_intermediate(c))
  {
    m.take();
    m.change_state(std::unique_ptr<VtParseStateCsiIntermediate>(new VtParseStateCsiIntermediate()));
  }
  else if(c == 0x3a || is_csi_private(c))
  {
    m.take();
    m.change_state(std::unique_ptr<VtParseStateCsiIgnore>(new VtParseStateCsiIgnore()));
  }
  else { throw_unhandled_char(c); }
}

void VtParseStateCsiIgnore::handle(char c, VtParseMachine& m)
{
  if(handle_anywhere_control_codes(c, m)) return;

  if(is_c0(c))
  {
    m.ignore();
    m.emit(c, VtSequence::Type::C0);
  }
  else if(c == Char::DEL) { m.ignore(); }
  else if(c >= 0x20 && c <= 0x3f) { m.take(); }
  else if(c >= 0x40 && c <= 0x7e)
  {
    m.take();
    m.emit(VtSequence::Type::Malformed);
    m.change_state(std::unique_ptr<VtParseStateGround>(new VtParseStateGround()));
  }
  else { throw_unhandled_char(c); }
}

void VtParseStateCsiIntermediate::handle(char c, VtParseMachine& m)
{
  if(handle_anywhere_control_codes(c, m)) return;

  if(is_c0(c))
  {
    m.ignore();
    m.emit(c, VtSequence::Type::C0);
  }
  else if(c == Char::DEL) { m.ignore(); }
  else if(is_intermediate(c)) { m.take(); }
  else if(c >= 0x40 && c <= 0x7e)
  {
    m.take();
    m.emit(VtSequence::Type::CSI);
    m.change_state(std::unique_ptr<VtParseStateGround>(new VtParseStateGround()));
  }
  else if(c >= 0x30 && c <= 0x3f)
  {
    m.take();
    m.change_state(std::unique_ptr<VtParseStateCsiIgnore>(new VtParseStateCsiIgnore()));
  }
  else
  {
    throw_unhandled_char(c);
    (void)m;
  }
}

void VtParseStateOsc::enter(VtParseMachine& m)
{
  /* Note about OSC processing
    *
    * https://vt100.net/emu/dec_ansi_parser describes the 'osc string' state to have
    * the following actions:
    *
    * entry/osc_start
    * event 20-7F/osc_put
    * exit/osc_end
    *
    * These are to support an external parser to handle parsing the control string.
    * Since this parser is simply extracting the OSC string from the stream (not
    * attempting to parse and interpret it), those actions will not be supported here.
    * Parsing the control string is a job for the consumer.
    *
    */
  (void)m;
}

void VtParseStateOsc::handle(char c, VtParseMachine& m)
{
  // Some UTF-8 continuation chars have some overlap with codes handled by handle_anywhere_control_codes(),
  // so consume UTF-8 first.
  if(handle_utf8(c, m)) return;

  // Store 8-bit OSC initiator
  if(m.collected().empty() && c == Char::OSC_8bit)
  {
    m.take();
    return;
  }

  // Some OSC terminators are handled by handle_anywhere_control_codes(), so handle terminators first
  if(c == Char::BEL || c == Char::ST || c == Char::ESC)
  {
    if(c == Char::ESC) { m.change_state(std::unique_ptr<VtParseStateEscape>(new VtParseStateEscape())); }
    else
    {
      m.take();
      m.change_state(std::unique_ptr<VtParseStateGround>(new VtParseStateGround()));
    }

    m.emit(VtSequence::Type::OSC);

    return;
  }

  if(handle_anywhere_control_codes(c, m))
  {
    // If a state transition is pending and we're still here, we have a malformed sequence.
    if(m.pending_transition()) { m.emit(VtSequence::Type::Malformed); }

    return;
  }

  if(is_c0(c)) { m.ignore(); }
  else if(c >= 0x20 && static_cast<unsigned char>(c) <= 0x7F) { m.take(); }
  else { throw_unhandled_char(c); }
}

void VtParseStateOsc::exit(VtParseMachine& m) { (void)m; }

void VtParseStateSos::handle(char c, VtParseMachine& m)
{
  if(handle_anywhere_control_codes(c, m)) return;

  if(is_c0(c)) { m.ignore(); }
  else if(c >= 0x20 && static_cast<unsigned char>(c) <= 0x7F) { m.take(); }
  else { throw_unhandled_char(c); }
}

class cmd_builder;
void invalidate_and_throw(cmd_builder& cmd, const char* reason);

/**
   * @brief Helper class for constructing a VtCommand
   */
class cmd_builder
{
  friend class Term::Private::VtCommand;

public:
  using params_type        = std::vector<int>;
  using intermediates_type = std::vector<char>;

  VtCommand::Type cmd() const noexcept { return m_cmd; }

  void cmd(VtCommand::Type cmd) noexcept { m_cmd = cmd; }

  char last_char() const noexcept { return m_last_char; }

  void last_char(char c) noexcept { m_last_char = c; }

  const params_type& params() const noexcept { return m_params; }

  const intermediates_type& intermediates() const noexcept { return m_intermediate; }

  void add_param(int param)
  {
    if(param < 0) { invalidate_and_throw(*this, "Negative parametera are not allowed"); }

    constexpr auto max = (std::numeric_limits<short>::max)();
    if(param > max) { invalidate_and_throw(*this, ("Parameter is too large - max is " + std::to_string(max)).c_str()); }

    m_params.push_back(param);
  }

  void clear_params() { m_params.clear(); }

  void pop_param_front() { m_params.erase(m_params.begin()); }

  void add_intermediate(char c) { m_intermediate.push_back(c); }

  void clear_intermediates() { m_intermediate.clear(); }

  bool has_intermediate(char c)
  {
    auto it = std::find(m_intermediate.begin(), m_intermediate.end(), c);
    if(it == m_intermediate.end()) return false;
    return true;
  }

private:
  VtCommand::Type    m_cmd{VtCommand::Type::Invalid};
  char               m_last_char{0};
  params_type        m_params;
  intermediates_type m_intermediate;
};

void invalidate_and_throw(cmd_builder& cmd, const char* reason)
{
  cmd.cmd(VtCommand::Type::Invalid);
  cmd.clear_params();
  cmd.clear_intermediates();
  throw std::invalid_argument(reason);
}

void csi_sgr(cmd_builder& cmd)
{
  if(cmd.last_char() != 'm') return;

  cmd.cmd(VtCommand::Type::Sgr);

  // If no params, 0 is the default.
  if(cmd.params().empty()) { cmd.add_param(0); }
  else if(cmd.params()[0] == 38 || cmd.params()[0] == 48)
  {
    if(cmd.params().size() < 3) invalidate_and_throw(cmd, "Required parameters are missing.");

    if(cmd.params()[1] == 2)
    {
      if(cmd.params().size() < 5) invalidate_and_throw(cmd, "One or more RGB parameters are missing.");

      for(cmd_builder::params_type::size_type i = 0; i < 3; ++i)
      {
        auto v = cmd.params()[i + 2];
        if(v < 0 || v > 255) invalidate_and_throw(cmd, "One or more RGB values are out of range.");
      }
    }
    else if(cmd.params()[1] == 5)
    {
      if(cmd.params().size() < 3) invalidate_and_throw(cmd, "Color table index value is missing.");

      auto v = cmd.params()[2];
      if(v < 0 || v > 255) invalidate_and_throw(cmd, "Color table index value is out of range.");
    }
    else
      invalidate_and_throw(cmd, "Second parameter must be 2 or 5.");
  }
}

void csi_cursor(cmd_builder& cmd)
{
  switch(cmd.last_char())
  {
    case 'A': cmd.cmd(VtCommand::Type::CursorUp); break;
    case 'B': cmd.cmd(VtCommand::Type::CursorDown); break;
    case 'C': cmd.cmd(VtCommand::Type::CursorRight); break;
    case 'D': cmd.cmd(VtCommand::Type::CursorLeft); break;
    case 'E': cmd.cmd(VtCommand::Type::CursorNextLine); break;
    case 'F': cmd.cmd(VtCommand::Type::CursorPreviousLine); break;
    case 'G': cmd.cmd(VtCommand::Type::CursorHorizontalAbsolute); break;
    case 'd': cmd.cmd(VtCommand::Type::CursorVerticalAbsolute); break;
    case 'H':
    case 'f': cmd.cmd(VtCommand::Type::CursorPositionAbsolute); break;
    case 's': cmd.cmd(VtCommand::Type::CursorSave); break;
    case 'u': cmd.cmd(VtCommand::Type::CursorRestore); break;
    case 'h':
    case 'l':
      if(cmd.has_intermediate('?') && !cmd.params().empty())
      {
        if(cmd.params()[0] == 12)
        {
          if(cmd.last_char() == 'h') { cmd.cmd(VtCommand::Type::CursorBlink); }
          else { cmd.cmd(VtCommand::Type::CursorNoBlink); }
        }
        else if(cmd.params()[0] == 25)
        {
          if(cmd.last_char() == 'h') { cmd.cmd(VtCommand::Type::CursorShow); }
          else { cmd.cmd(VtCommand::Type::CursorHide); }
        }
      }
      break;
    case 'q':
      if(cmd.has_intermediate(0x20))
      {
        int p = cmd.params().empty() ? 0 : cmd.params()[0];
        switch(p)
        {
          case 0: cmd.cmd(VtCommand::Type::CursorDefaultShape); break;
          case 1: cmd.cmd(VtCommand::Type::CursorBlinkingBlock); break;
          case 2: cmd.cmd(VtCommand::Type::CursorSteadyBlock); break;
          case 3: cmd.cmd(VtCommand::Type::CursorBlinkingUnderline); break;
          case 4: cmd.cmd(VtCommand::Type::CursorSteadyUnderline); break;
          case 5: cmd.cmd(VtCommand::Type::CursorBlinkingBar); break;
          case 6: cmd.cmd(VtCommand::Type::CursorSteadyBar); break;
        }
      }
      break;
  }
}

void csi_viewport_position(cmd_builder& cmd)
{
  switch(cmd.last_char())
  {
    case 'S': cmd.cmd(VtCommand::Type::ViewportScrollUp); break;
    case 'T': cmd.cmd(VtCommand::Type::ViewportScrollDown); break;
  }
}

void csi_text_modification(cmd_builder& cmd)
{
  switch(cmd.last_char())
  {
    case '@': cmd.cmd(VtCommand::Type::TextInsertCharacter); break;
    case 'P': cmd.cmd(VtCommand::Type::TextDeleteCharacter); break;
    case 'X': cmd.cmd(VtCommand::Type::TextEraseCharacter); break;
    case 'L': cmd.cmd(VtCommand::Type::TextInsertLine); break;
    case 'M': cmd.cmd(VtCommand::Type::TextDeleteLine); break;
    case 'J':
    case 'K':
      for(const auto p: cmd.params())
      {
        // 3 was added in Linux 3.0
        if(cmd.last_char() == 'J' && p > 3) { invalidate_and_throw(cmd, "0, 1, 2 and 3 are the only valid parameter values"); }
        else if(cmd.last_char() == 'K' && p > 2) { invalidate_and_throw(cmd, "0, 1 and 2 are the only valid parameter values"); }
      }

      if(cmd.last_char() == 'J') cmd.cmd(VtCommand::Type::TextEraseInDisplay);
      else
        cmd.cmd(VtCommand::Type::TextEraseInLine);
      break;
  }
}

void csi_mode_changes(cmd_builder& cmd)
{
  switch(cmd.last_char())
  {
    case 'h':
    case 'l':
      if(cmd.has_intermediate('?') && !cmd.params().empty())
      {
        if(cmd.params()[0] == 1)
        {
          if(cmd.last_char() == 'h') { cmd.cmd(VtCommand::Type::ModeCursorKeysApplication); }
          else { cmd.cmd(VtCommand::Type::ModeCursorKeysNumeric); }
        }
      }
      break;
  }
}

void csi_query_state(cmd_builder& cmd)
{
  switch(cmd.last_char())
  {
    case 'n':
      if(!cmd.params().empty() && cmd.params()[0] == 6)
      {
        cmd.cmd(VtCommand::Type::QueryCursorPosition);
        cmd.clear_params();
      }
      break;
    case 'c':
      if(!cmd.params().empty() && cmd.params()[0] == 0)
      {
        cmd.cmd(VtCommand::Type::QueryDeviceAttributes);
        cmd.clear_params();
      }
      break;
  }
}

void csi_tabs(cmd_builder& cmd)
{
  switch(cmd.last_char())
  {
    case 'I': cmd.cmd(VtCommand::Type::TabCursorForward); break;
    case 'Z': cmd.cmd(VtCommand::Type::TabCursorBackward); break;
    case 'g':
      if(!cmd.params().empty())
      {
        if(cmd.params()[0] == 0)
        {
          cmd.cmd(VtCommand::Type::TabCursorClearCurrent);
          cmd.clear_params();
        }
        else if(cmd.params()[0] == 3)
        {
          cmd.cmd(VtCommand::Type::TabClearAll);
          cmd.clear_params();
        }
      }
      break;
  }
}

void csi_scrolling_margins(cmd_builder& cmd)
{
  switch(cmd.last_char())
  {
    case 'r': cmd.cmd(VtCommand::Type::ScrollingRegionSet); break;
  }
}

void csi_screen_buffer(cmd_builder& cmd)
{
  switch(cmd.last_char())
  {
    case 'h':
    case 'l':
      if(cmd.has_intermediate('?') && !cmd.params().empty())
      {
        if(cmd.params()[0] == 1049)
        {
          if(cmd.last_char() == 'h') { cmd.cmd(VtCommand::Type::ScreenBufferAlternate); }
          else { cmd.cmd(VtCommand::Type::ScreenBufferMain); }
        }
      }
      break;
  }
}

void csi_window_width(cmd_builder& cmd)
{
  switch(cmd.last_char())
  {
    case 'h':
    case 'l':
      if(cmd.has_intermediate('?') && !cmd.params().empty())
      {
        if(cmd.params()[0] == 3)
        {
          if(cmd.last_char() == 'h') { cmd.cmd(VtCommand::Type::WindowWidth132); }
          else { cmd.cmd(VtCommand::Type::WindowWidth80); }
        }
      }
      break;
  }
}

void csi_soft_reset(cmd_builder& cmd)
{
  switch(cmd.last_char())
  {
    case 'p':
      if(cmd.has_intermediate('!')) { cmd.cmd(VtCommand::Type::SoftReset); }
      break;
  }
}

void csi_exclusive(cmd_builder& cmd)
{
  switch(cmd.last_char())
  {
    case 'h':
    case 'l':
      if(cmd.has_intermediate('?') && !cmd.params().empty())
      {
        if(cmd.params()[0] == 3210)
        {
          if(cmd.last_char() == 'h') { cmd.cmd(VtCommand::Type::EnableErrorReporting); }
          else { cmd.cmd(VtCommand::Type::DisableErrorReporting); }
        }
      }
      break;
  }
}

void csi_finalize(cmd_builder& cmd)
{
  std::vector<std::function<void(cmd_builder&)>> funcs{csi_sgr, csi_cursor, csi_viewport_position, csi_text_modification, csi_mode_changes, csi_query_state, csi_tabs, csi_scrolling_margins, csi_screen_buffer, csi_window_width, csi_soft_reset, csi_exclusive};

  for(auto& f: funcs)
  {
    f(cmd);
    if(cmd.cmd() != VtCommand::Type::Invalid) break;
  }
}

cmd_builder parse_csi(const VtSequence& seq)
{
  cmd_builder cmd;
  auto&       str = seq.content();

  if(!seq.content().empty())
  {
    try
    {
      size_t      i{0};
      std::string param_builder;

      // Params
      char c = str[0];
      for(; i < str.size(); ++i)
      {
        c = str[i];

        if(!is_csi_private(c) && !is_csi_param(c)) { break; }

        if(is_csi_private(c))
        {
          if(i != 0) { throw std::invalid_argument("Private marker may only appear as the first character"); }
          cmd.add_intermediate(c);
        }
        else if(c == ';')
        {
          if(!param_builder.empty())
          {
            cmd.add_param(std::stoi(param_builder));
            param_builder.clear();
          }
          else { cmd.add_param(0); }
        }
        else { param_builder.push_back(c); }
      }

      // Add final param if appropriate.
      if(!param_builder.empty())
      {
        cmd.add_param(std::stoi(param_builder));
        param_builder.clear();
      }
      else if(i > 0 && str[i - 1] == ';') { cmd.add_param(0); }

      // Intermediates
      for(; i < str.size(); ++i)
      {
        c = str[i];
        if(!is_intermediate(c)) break;
        cmd.add_intermediate(c);
      }

      // Final character
      if(i == str.size() - 1 && (c >= 0x40 && c <= 0x7e))
      {
        cmd.last_char(c);
        csi_finalize(cmd);
      }
      else { throw std::invalid_argument("Final character is invalid or it's in the wrong position."); }
    }
    catch(const std::invalid_argument& e)
    {
      (void)e;
      cmd.cmd(VtCommand::Type::Invalid);
    }
  }

  return cmd;
}

cmd_builder parse_esc(const VtSequence& seq)
{
  cmd_builder cmd;

  if(seq.content().empty()) return cmd;

  switch(seq.content()[0])
  {
    case 'M':
      cmd.cmd(VtCommand::Type::CursorUp);
      cmd.add_param(1);
      break;
    case '7': cmd.cmd(VtCommand::Type::CursorSave); break;
    case '8': cmd.cmd(VtCommand::Type::CursorRestore); break;
    case '=': cmd.cmd(VtCommand::Type::ModeKeypadApplication); break;
    case '>': cmd.cmd(VtCommand::Type::ModeKeypadNumeric); break;
    case 'H': cmd.cmd(VtCommand::Type::TabCursorSet); break;
    case '(':
      if(seq.content().size() == 2)
      {
        if(seq.content()[1] == '0') { cmd.cmd(VtCommand::Type::CharsetDecLineDrawing); }
        else if(seq.content()[1] == 'B') { cmd.cmd(VtCommand::Type::CharsetUsAscii); }
      }
      break;
    case '\\': cmd.cmd(VtCommand::Type::StringTerminator); break;
  }

  return cmd;
}

cmd_builder parse_osc(const VtSequence& seq)
{
  cmd_builder cmd;

  // Currently, there are only 3 supported OSC sequences and they are all of the form '<n>;'
  // where <n> is 0, 2 or 4.
  if(seq.content().size() < 3 || seq.content()[1] != ';') return cmd;

  auto c = seq.content()[0];
  if(c == '0' || c == '2')
  {
    cmd.cmd(VtCommand::Type::WindowTitleSet);

    /*
      * The window title is stored in the intermediates as a workaround for
      * VtCommand's design that uses integers as the parameters' type. It's
      * not ideal, but since the use of OSC is so limited (it's only used for
      * setting window title and RGB palette), it did not seem worth any extra
      * complexity to devise a more robust solution.
      */
    for(size_t i = 2; i < seq.content().size(); ++i) { cmd.add_intermediate(seq.content()[i]); }
  }
  else if(c == '4' && seq.content().size() >= 13)
  {
    try
    {
      const auto& str = seq.content();

      // Palette index
      auto pos = str.find(';', 2);
      if(pos == std::string::npos) throw std::invalid_argument("Expected ';'");
      auto val = std::stoi(str.substr(2, pos - 2));
      if(val < 0 || val > 15) throw std::invalid_argument("Color palette index is out of range");
      cmd.add_param(val);

      // Verify the string 'rgb:' is included
      auto end_pos = str.find("rgb:");
      if(end_pos != pos + 1) throw std::invalid_argument("Expected 'rgb:'");

      auto add_rgb = [&cmd, &str](size_t b, size_t e)
      {
        auto rbg = std::stoi(str.substr(b, e));
        if(rbg < 0 || rbg > 255) throw std::invalid_argument("RGB value is out of range");
        cmd.add_param(rbg);
      };

      // R value
      pos     = end_pos + 4;
      end_pos = str.find('/', pos);
      if(pos == std::string::npos) throw std::invalid_argument("Expected '/' after R value");
      add_rgb(pos, end_pos - pos);

      // G value
      pos     = end_pos + 1;
      end_pos = str.find('/', pos);
      if(pos == std::string::npos) throw std::invalid_argument("Expected '/' after G value");
      add_rgb(pos, end_pos - pos);

      // B value
      if(str.size() <= end_pos + 1) throw std::invalid_argument("B value is missing");
      add_rgb(end_pos + 1, std::string::npos);

      cmd.cmd(VtCommand::Type::ScreenColorPaletteSet);
    }
    catch(const std::invalid_argument& e)
    {
      (void)e;
      cmd.cmd(VtCommand::Type::Invalid);
    }
  }

  return cmd;
}

std::uint8_t win_4bit_to_ansi_base(uint8_t four_bit)
{
  switch(four_bit)
  {
    case 0: return 0;    // Black
    case 1: return 4;    // Blue
    case 2: return 2;    // Green
    case 3: return 6;    // Cyan
    case 4: return 1;    // Red
    case 5: return 5;    // Magenta
    case 6: return 3;    // Yellow
    case 7: return 7;    // White
    case 8: return 60;   // Bright Black (Gray)
    case 9: return 64;   // Bright Blue
    case 10: return 62;  // Bright Green
    case 11: return 66;  // Bright Cyan
    case 12: return 61;  // Bright Red
    case 13: return 65;  // Bright Magenta
    case 14: return 63;  // Bright Yellow
    case 15: return 67;  // Bright White
    default: return 0;
  }
}

std::uint8_t win2ansi_fg(unsigned short attrib)
{
  auto val = static_cast<std::uint8_t>(0x0f & attrib);
  return static_cast<std::uint8_t>(win_4bit_to_ansi_base(val) + 30);
}

std::uint8_t win2ansi_bg(unsigned short attrib)
{
  std::uint8_t val = static_cast<std::uint8_t>((0xf0 & attrib) >> 4);
  return static_cast<std::uint8_t>(win_4bit_to_ansi_base(val) + 40);
}

#if !defined(_WIN32)
  #define FOREGROUND_BLUE      0x0001  // text color contains blue.
  #define FOREGROUND_GREEN     0x0002  // text color contains green.
  #define FOREGROUND_RED       0x0004  // text color contains red.
  #define FOREGROUND_INTENSITY 0x0008  // text color is intensified.
  #define BACKGROUND_BLUE      0x0010  // background color contains blue.
  #define BACKGROUND_GREEN     0x0020  // background color contains green.
  #define BACKGROUND_RED       0x0040  // background color contains red.
  #define BACKGROUND_INTENSITY 0x0080  // background color is intensified.
#endif

unsigned short ansi2win(std::uint8_t code)
{
  bool           fg{true};
  bool           bright{true};
  int            base_code{};
  unsigned short attrib{};

  if(code >= 100 && code <= 107)
  {
    fg        = false;
    bright    = true;
    base_code = code - 100;
  }
  if(code >= 90 && code <= 97)
  {
    fg        = true;
    bright    = true;
    base_code = code - 90;
  }
  if(code >= 40 && code <= 47)
  {
    fg        = false;
    bright    = false;
    base_code = code - 40;
  }
  if(code >= 30 && code <= 37)
  {
    fg        = true;
    bright    = false;
    base_code = code - 30;
  }

  switch(base_code)
  {
    case 0:  // Black
      attrib = 0;
      break;
    case 1:  // Red
      attrib = fg ? FOREGROUND_RED : BACKGROUND_RED;
      break;
    case 2:  // Green
      attrib = fg ? FOREGROUND_GREEN : BACKGROUND_GREEN;
      break;
    case 3:  // Yellow
      attrib = fg ? FOREGROUND_RED | FOREGROUND_GREEN : BACKGROUND_RED | BACKGROUND_GREEN;
      break;
    case 4:  // Blue
      attrib = fg ? FOREGROUND_BLUE : BACKGROUND_BLUE;
      break;
    case 5:  // Magenta
      attrib = fg ? FOREGROUND_BLUE | FOREGROUND_RED : BACKGROUND_BLUE | BACKGROUND_RED;
      break;
    case 6:  // Cyan
      attrib = fg ? FOREGROUND_BLUE | FOREGROUND_GREEN : BACKGROUND_BLUE | BACKGROUND_GREEN;
      break;
    case 7:  // White
      attrib = fg ? FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE : BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
      break;
  }

  if(bright) { attrib |= fg ? FOREGROUND_INTENSITY : BACKGROUND_INTENSITY; }

  return attrib;
}

const char* to_char_str(VtCommand::Type type)
{
  switch(type)
  {
    case VtCommand::Type::Invalid: return "Invalid";
    case VtCommand::Type::CursorUp: return "CursorUp";
    case VtCommand::Type::CursorDown: return "CursorDown";
    case VtCommand::Type::CursorRight: return "CursorRight";
    case VtCommand::Type::CursorLeft: return "CursorLeft";
    case VtCommand::Type::CursorPreviousLine: return "CursorPreviousLine";
    case VtCommand::Type::CursorNextLine: return "CursorNextLine";
    case VtCommand::Type::CursorHorizontalAbsolute: return "CursorHorizontalAbsolute";
    case VtCommand::Type::CursorVerticalAbsolute: return "CursorVerticalAbsolute";
    case VtCommand::Type::CursorPositionAbsolute: return "CursorPositionAbsolute";
    case VtCommand::Type::CursorSave: return "CursorSave";
    case VtCommand::Type::CursorRestore: return "CursorRestore";
    case VtCommand::Type::CursorBlink: return "CursorBlink";
    case VtCommand::Type::CursorNoBlink: return "CursorNoBlink";
    case VtCommand::Type::CursorHide: return "CursorHide";
    case VtCommand::Type::CursorShow: return "CursorShow";
    case VtCommand::Type::CursorDefaultShape: return "CursorDefaultShape";
    case VtCommand::Type::CursorBlinkingBlock: return "CursorBlinkingBlock";
    case VtCommand::Type::CursorSteadyBlock: return "CursorSteadyBlock";
    case VtCommand::Type::CursorBlinkingUnderline: return "CursorBlinkingUnderline";
    case VtCommand::Type::CursorSteadyUnderline: return "CursorSteadyUnderline";
    case VtCommand::Type::CursorBlinkingBar: return "CursorBlinkingBar";
    case VtCommand::Type::CursorSteadyBar: return "CursorSteadyBar";
    case VtCommand::Type::ViewportScrollUp: return "ViewportScrollUp";
    case VtCommand::Type::ViewportScrollDown: return "ViewportScrollDown";
    case VtCommand::Type::TextInsertCharacter: return "TextInsertCharacter";
    case VtCommand::Type::TextDeleteCharacter: return "TextDeleteCharacter";
    case VtCommand::Type::TextEraseCharacter: return "TextEraseCharacter";
    case VtCommand::Type::TextInsertLine: return "TextInsertLine";
    case VtCommand::Type::TextDeleteLine: return "TextDeleteLine";
    case VtCommand::Type::TextEraseInDisplay: return "TextEraseInDisplay";
    case VtCommand::Type::TextEraseInLine: return "TextEraseInLine";
    case VtCommand::Type::Sgr: return "Sgr";
    case VtCommand::Type::ScreenColorPaletteSet: return "ScreenColorPaletteSet";
    case VtCommand::Type::ModeKeypadApplication: return "ModeKeypadApplication";
    case VtCommand::Type::ModeKeypadNumeric: return "ModeKeypadNumeric";
    case VtCommand::Type::ModeCursorKeysApplication: return "ModeCursorKeysApplication";
    case VtCommand::Type::ModeCursorKeysNumeric: return "ModeCursorKeysNumeric";
    case VtCommand::Type::QueryCursorPosition: return "QueryCursorPosition";
    case VtCommand::Type::QueryDeviceAttributes: return "QueryDeviceAttributes";
    case VtCommand::Type::TabCursorSet: return "TabCursorSet";
    case VtCommand::Type::TabCursorForward: return "TabCursorForward";
    case VtCommand::Type::TabCursorBackward: return "TabCursorBackward";
    case VtCommand::Type::TabCursorClearCurrent: return "TabCursorClearCurrent";
    case VtCommand::Type::TabClearAll: return "TabClearAll";
    case VtCommand::Type::CharsetDecLineDrawing: return "CharsetDecLineDrawing";
    case VtCommand::Type::CharsetUsAscii: return "CharsetUsAscii";
    case VtCommand::Type::ScrollingRegionSet: return "ScrollingRegionSet";
    case VtCommand::Type::WindowTitleSet: return "WindowTitleSet";
    case VtCommand::Type::ScreenBufferAlternate: return "ScreenBufferAlternate";
    case VtCommand::Type::ScreenBufferMain: return "ScreenBufferMain";
    case VtCommand::Type::WindowWidth132: return "WindowWidth132";
    case VtCommand::Type::WindowWidth80: return "WindowWidth80";
    case VtCommand::Type::SoftReset: return "SoftReset";
    case VtCommand::Type::StringTerminator: return "StringTerminator";
    case VtCommand::Type::EnableErrorReporting: return "EnableErrorReporting";
    case VtCommand::Type::DisableErrorReporting: return "DisableErrorReporting";
  }

  return "???";
}

const char* to_char_str(VtSequence::Type type)
{
  switch(type)
  {
    case VtSequence::Type::Text: return "Text";
    case VtSequence::Type::Malformed: return "Malformed";
    case VtSequence::Type::C0: return "C0";
    case VtSequence::Type::C1: return "C1";
    case VtSequence::Type::Escape: return "Escape";
    case VtSequence::Type::CSI: return "CSI";
    case VtSequence::Type::OSC: return "OSC";
  }

  return "???";
}

std::string to_string(const VtSequence& seq)
{
  std::stringstream ss;

  ss << "Sequence type '" << to_char_str(seq.type()) << "', content: '" << seq.content() << "'";

  return ss.str();
}

std::string to_string(const VtCommand& cmd)
{
  std::stringstream ss;

  ss << "Command type '" << to_char_str(cmd.type()) << "'";

  ss << " params: ";
  if(cmd.params().empty()) ss << "none";
  else
  {
    ss << "{";
    std::copy(cmd.params().begin(), cmd.params().end() - 1, std::ostream_iterator<int>(ss, ","));
    ss << cmd.params().back();
    ss << "}";
  }

  ss << " intermediates: ";
  if(cmd.intermediates().empty()) ss << "none";
  else
  {
    ss << "[";
    std::copy(cmd.intermediates().begin(), cmd.intermediates().end() - 1, std::ostream_iterator<int>(ss, ","));
    ss << cmd.intermediates().back();
    ss << "]";
  }

  return ss.str();
}

#if defined(_WIN32)
/*
  * The following functions are not available in Windows releases prior to Vista. Most
  * of the other console API functions are available as far back as Windows 2000
  * Professional and Windows 2000 Server.
  *
  * In order to support those earlier versions with most of the functionality, these
  * functions are not statically linked.
  *
  * - GetConsoleScreenBufferInfoEx
  * - SetConsoleScreenBufferInfoEx
  * - SetCurrentConsoleFontEx
  */

// Duplicate of CONSOLE_SCREEN_BUFFER_INFOEX
typedef struct _CSBIX_DUP
{
  ULONG      cbSize;
  COORD      dwSize;
  COORD      dwCursorPosition;
  WORD       wAttributes;
  SMALL_RECT srWindow;
  COORD      dwMaximumWindowSize;
  WORD       wPopupAttributes;
  BOOL       bFullscreenSupported;
  COLORREF   ColorTable[16];
} CSBIX_DUP, *PCSBIX_DUP;

// GetConsoleScreenBufferInfoEx & SetConsoleScreenBufferInfoEx
std::function<BOOL(HANDLE, PCSBIX_DUP)> csbix_get;
std::function<BOOL(HANDLE, PCSBIX_DUP)> csbix_set;

// Duplicate of CONSOLE_FONT_INFOEX
typedef struct _CFIX_DUP
{
  ULONG cbSize;
  DWORD nFont;
  COORD dwFontSize;
  UINT  FontFamily;
  UINT  FontWeight;
  WCHAR FaceName[LF_FACESIZE];
} CFIX_DUP, *PCFIX_DUP;

// SetCurrentConsoleFontEx
std::function<BOOL(HANDLE, BOOL, PCFIX_DUP)> cfix_set;
#endif

#if defined(_WIN32)
void throw_win32_error(const char* detail)
{
  auto        ec   = GetLastError();
  std::string what = detail;
  what += " failed with code ";
  what += std::to_string(ec);
  throw std::runtime_error(what);
}
#endif

vt::Handle terminal_handle_open_default(VtApi::TerminalHandleType type, std::function<void(vt::Handle::raw_handle_type)> closer)
{
#if defined(_WIN32)
  vt::Handle::raw_handle_type handle{};

  switch(type)
  {
    case VtApi::TerminalHandleType::OutPrimary:
      handle = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

      if(handle == INVALID_HANDLE_VALUE) { throw_win32_error("CreateFile (CONOUT$)"); }
      break;
    case VtApi::TerminalHandleType::OutAlternate:
      handle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

      if(handle == INVALID_HANDLE_VALUE) { throw_win32_error("CreateConsoleScreenBuffer"); }

      break;
    case VtApi::TerminalHandleType::StdIn:
      handle = GetStdHandle(STD_INPUT_HANDLE);
      if(handle == INVALID_HANDLE_VALUE) { throw_win32_error("GetStdHandle(STD_INPUT_HANDLE)"); }
      break;
  }

  return vt::Handle(handle, closer);
#else
  (void)type;
  throw std::runtime_error("The function 'terminal_handle_open_default' must be replaced with another function in a non-Windows environment.");
#endif
}

void terminal_handle_close_default(vt::Handle::raw_handle_type handle)
{
#if defined(_WIN32)
  if(handle != nullptr && handle != INVALID_HANDLE_VALUE) { CloseHandle(handle); }
#else
  (void)handle;
  throw std::runtime_error("The function 'terminal_handle_close_default' must be replaced with another function in a non-Windows environment.");
#endif
}

vt::state terminal_get_state_default(const vt::Handle& handle)
{
  vt::state state;
#if defined(_WIN32)

  CONSOLE_SCREEN_BUFFER_INFO csbi{};
  if(!GetConsoleScreenBufferInfo(handle, &csbi)) { throw_win32_error("GetConsoleScreenBufferInfo"); }

  state.screen_buffer.height = static_cast<vt::dim_type>(csbi.dwSize.Y);
  state.screen_buffer.width  = static_cast<vt::dim_type>(csbi.dwSize.X);

  auto col              = Term::Column(static_cast<std::uint16_t>(csbi.dwCursorPosition.X + 1));
  auto row              = Term::Row(static_cast<std::uint16_t>(csbi.dwCursorPosition.Y + 1));
  state.cursor_position = Term::Cursor({col, row});

  state.foreground_color = win2ansi_fg(csbi.wAttributes);
  state.background_color = win2ansi_bg(csbi.wAttributes);

  state.window.top    = static_cast<vt::dim_type>(csbi.srWindow.Top + 1);
  state.window.right  = static_cast<vt::dim_type>(csbi.srWindow.Right + 1);
  state.window.bottom = static_cast<vt::dim_type>(csbi.srWindow.Bottom + 1);
  state.window.left   = static_cast<vt::dim_type>(csbi.srWindow.Left + 1);

  return state;
#else
  (void)handle;
  win2ansi_fg(0);  // avoid warning unreferenced function with internal linkage has been removed
  win2ansi_bg(0);
  ansi2win(0);
  throw std::runtime_error("The function 'terminal_get_state_default' must be replaced with another function in a non-Windows environment.");
#endif
}

void terminal_scroll_default(const vt::Handle& handle, const vt::rectangle& scroll, const vt::rectangle& clip, const vt::point& new_top_left, const vt::fill& fill)
{
#if defined(_WIN32)
  SMALL_RECT sr{};
  sr.Top    = scroll.top - 1;
  sr.Right  = scroll.right - 1;
  sr.Bottom = scroll.bottom - 1;
  sr.Left   = scroll.left - 1;

  SMALL_RECT cr{};
  cr.Top    = clip.top - 1;
  cr.Right  = clip.right - 1;
  cr.Bottom = clip.bottom - 1;
  cr.Left   = clip.left - 1;

  COORD dst{};
  dst.X = static_cast<SHORT>(new_top_left.x) - 1;
  dst.Y = static_cast<SHORT>(new_top_left.y) - 1;

  // Define the fill character and color for cells left empty by the move
  CHAR_INFO ci{};
  ci.Char.UnicodeChar = std::btowc(fill.c);
  ci.Attributes       = static_cast<WORD>(ansi2win(fill.foreground_color) | ansi2win(fill.background_color));

  if(!ScrollConsoleScreenBufferW(handle, &sr, &cr, dst, &ci)) { throw_win32_error("ScrollConsoleScreenBuffer"); }
#else
  (void)handle;
  (void)scroll;
  (void)clip;
  (void)new_top_left;
  (void)fill;
  throw std::runtime_error("The function 'terminal_scroll_default' must be replaced with another function in a non-Windows environment.");
#endif
}

void terminal_fill_default(const vt::Handle& handle, const vt::fill& fill, const vt::point& at, std::size_t len)
{
#if defined(_WIN32)
  COORD write_at{};
  write_at.X = static_cast<SHORT>(at.x) - 1;
  write_at.Y = static_cast<SHORT>(at.y) - 1;

  WORD attribs = static_cast<WORD>(ansi2win(fill.foreground_color) | ansi2win(fill.background_color));

  WCHAR fill_char = std::btowc(fill.c);
  DWORD written{};
  if(!FillConsoleOutputCharacterW(handle, fill_char, static_cast<DWORD>(len), write_at, &written)) { throw_win32_error("FillConsoleOutputCharacter"); }

  if(!FillConsoleOutputAttribute(handle, attribs, static_cast<DWORD>(len), write_at, &written)) { throw_win32_error("FillConsoleOutputAttribute"); }
#else
  (void)handle;
  (void)fill;
  (void)at;
  (void)len;
  throw std::runtime_error("The function 'terminal_fill_default' must be replaced with another function in a non-Windows environment.");
#endif
}

void terminal_set_cursor_position_default(const vt::Handle& handle, const Term::Cursor& cursor)
{
#if defined(_WIN32)
  COORD coord{};
  coord.X = static_cast<SHORT>(cursor.column()) - 1;
  coord.Y = static_cast<SHORT>(cursor.row()) - 1;

  if(!SetConsoleCursorPosition(handle, coord)) { throw_win32_error("SetConsoleCursorPosition"); }

#else
  (void)handle;
  (void)cursor;
  throw std::runtime_error("The function 'terminal_set_cursor_position_default' must be replaced with another function in a non-Windows environment.");
#endif
}

vt::cursor_appearance terminal_get_cursor_appreance_default(const vt::Handle& handle)
{
#if defined(_WIN32)
  CONSOLE_CURSOR_INFO info{};
  if(!GetConsoleCursorInfo(handle, &info)) { throw_win32_error("GetConsoleCursorInfo"); }

  vt::cursor_appearance ca;
  ca.fill_percent = static_cast<vt::tiny_type>(info.dwSize);
  ca.visible      = info.bVisible;

  return ca;
#else
  (void)handle;
  throw std::runtime_error("The function 'terminal_get_cursor_appreance_default' must be replaced with another function in a non-Windows environment.");
#endif
}

void terminal_set_cursor_appreance_default(const vt::Handle& handle, const vt::cursor_appearance& ca)
{
#if defined(_WIN32)
  CONSOLE_CURSOR_INFO info{};
  info.dwSize   = ca.fill_percent;
  info.bVisible = ca.visible;

  if(!SetConsoleCursorInfo(handle, &info)) { throw_win32_error("SetConsoleCursorInfo"); }
#else
  (void)handle;
  (void)ca;
  throw std::runtime_error("The function 'terminal_set_cursor_appreance_default' must be replaced with another function in a non-Windows environment.");
#endif
}

vt::screen_buffer_defaults terminal_get_screen_buffer_defaults_default(const vt::Handle& handle)
{
#if defined(_WIN32)
  if(!csbix_get)
  {
    auto mh = GetModuleHandleW(L"kernel32.dll");
    if(mh)
    {
  #pragma warning(push)
  #pragma warning(disable : 4191)
      using ft_csbix = BOOL(WINAPI*)(HANDLE, PCSBIX_DUP);
      csbix_get      = reinterpret_cast<ft_csbix>(GetProcAddress(mh, "GetConsoleScreenBufferInfoEx"));
      csbix_set      = reinterpret_cast<ft_csbix>(GetProcAddress(mh, "SetConsoleScreenBufferInfoEx"));

      using ft_cfix = BOOL(WINAPI*)(HANDLE, BOOL, PCFIX_DUP);
      cfix_set      = reinterpret_cast<ft_cfix>(GetProcAddress(mh, "SetCurrentConsoleFontEx"));
  #pragma warning(pop)
    }

    if(!csbix_get)
    {
      csbix_get = [](HANDLE, PCSBIX_DUP) -> BOOL { return false; };
    }

    if(!csbix_set)
    {
      csbix_set = [](HANDLE, PCSBIX_DUP) -> BOOL { return false; };
    }

    if(!cfix_set)
    {
      cfix_set = [](HANDLE, BOOL, PCFIX_DUP) -> BOOL { return false; };
    }
  }

  vt::screen_buffer_defaults defaults;

  CONSOLE_CURSOR_INFO cursor_info{};
  if(GetConsoleCursorInfo(handle, &cursor_info)) { defaults.cursor_fill_percent = static_cast<vt::tiny_type>(cursor_info.dwSize); }

  WORD      attributes{};
  CSBIX_DUP csbix{};
  csbix.cbSize = sizeof(CSBIX_DUP);
  if(csbix_get(handle, &csbix)) { attributes = csbix.wAttributes; }
  else
  {
    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    if(!GetConsoleScreenBufferInfo(handle, &csbi)) { throw_win32_error("GetConsoleScreenBufferInfo"); }

    attributes = csbi.wAttributes;
  }

  defaults.foreground_color = win2ansi_fg(attributes);
  defaults.background_color = win2ansi_bg(attributes);

  return defaults;
#else
  (void)handle;
  throw std::runtime_error("The function 'terminal_get_screen_buffer_defaults_default' must be replaced with another function in a non-Windows environment.");
#endif
}

void terminal_set_text_attributes_default(const vt::Handle& handle, const vt::text_attributes& attributes)
{
#if defined(_WIN32)
  auto attrib = static_cast<WORD>(ansi2win(attributes.foreground_color) | ansi2win(attributes.background_color));

  if(!SetConsoleTextAttribute(handle, attrib)) { throw_win32_error("FillConsoleOutputAttribute"); }
#else
  (void)handle;
  (void)attributes;
  throw std::runtime_error("The function 'terminal_set_text_attributes_default' must be replaced with another function in a non-Windows environment.");
#endif
}

void terminal_write_to_input_default(const vt::Handle& handle, const std::string& data)
{
#if defined(_WIN32)
  std::vector<INPUT_RECORD> inputs;
  DWORD                     out{};

  for(auto c: data)
  {
    INPUT_RECORD r{};
    r.EventType                        = KEY_EVENT;
    r.Event.KeyEvent.wRepeatCount      = 1;
    r.Event.KeyEvent.uChar.UnicodeChar = static_cast<WCHAR>(c);
    r.Event.KeyEvent.bKeyDown          = TRUE;
    inputs.push_back(r);

    r.Event.KeyEvent.bKeyDown = FALSE;
    inputs.push_back(r);
  }

  if(!WriteConsoleInputW(handle, inputs.data(), static_cast<DWORD>(inputs.size()), &out)) { throw_win32_error("WriteConsoleInput"); }
#else
  (void)handle;
  (void)data;
  throw std::runtime_error("The function 'terminal_write_to_input_default' must be replaced with another function in a non-Windows environment.");
#endif
}

void terminal_set_palette_color_default(const vt::Handle& handle, vt::tiny_type index, const vt::rgb& rgb)
{
#if defined(_WIN32)
  CSBIX_DUP csbix{};
  csbix.cbSize = sizeof(CSBIX_DUP);
  if(!csbix_get(handle, &csbix)) { throw_win32_error("GetConsoleScreenBufferInfoEx"); }

  csbix.ColorTable[index] = RGB(rgb.r, rgb.g, rgb.b);

  if(!csbix_set(handle, &csbix)) { throw_win32_error("SetConsoleScreenBufferInfoEx"); }
#else
  (void)handle;
  (void)index;
  (void)rgb;
  throw std::runtime_error("The function 'terminal_set_palette_color_default' must be replaced with another function in a non-Windows environment.");
#endif
}

std::wstring to_wide(const std::string& utf8)
{
#if defined(_WIN32)
  DWORD flags{0};

  auto utf8_size = static_cast<int>(utf8.size());

  auto wide_size = MultiByteToWideChar(CP_UTF8, flags, utf8.data(), utf8_size, nullptr, 0);
  if(wide_size == 0) { throw_win32_error("MultiByteToWideChar (calculate size pass)"); }

  std::wstring wide(static_cast<std::size_t>(wide_size), L'\0');

  auto rtn = MultiByteToWideChar(CP_UTF8, flags, utf8.data(), utf8_size, &wide[0], wide_size);
  if(rtn == 0) { throw_win32_error("MultiByteToWideChar (convert pass)"); }

  return wide;
#else
  (void)utf8;
  throw std::runtime_error("The function 'to_wide' is not implemented in a non-Windows environment.");
#endif
}

void terminal_write_to_output_default(const vt::Handle& handle, const std::string& utf8)
{
#if defined(_WIN32)
  if(utf8.empty()) return;

  auto wide = to_wide(utf8);

  DWORD written{0};
  if(!WriteConsoleW(handle, wide.data(), static_cast<DWORD>(wide.size()), &written, nullptr)) { throw_win32_error("WriteConsoleW"); }
#else
  (void)handle;
  (void)utf8;
  throw std::runtime_error("The function 'terminal_write_to_output_default' must be replaced with another function in a non-Windows environment.");
#endif
}

void terminal_set_title_default(const vt::Handle& handle, const std::string& title)
{
  (void)handle;
#if defined(_WIN32)
  if(title.empty()) return;

  auto wide = to_wide(title);
  SetConsoleTitleW(wide.c_str());
#else
  (void)title;
  throw std::runtime_error("The function 'terminal_set_title_default' must be replaced with another function in a non-Windows environment.");
#endif
}

void terminal_set_active_screen_buffer_default(const vt::Handle& handle)
{
#if defined(_WIN32)
  if(!SetConsoleActiveScreenBuffer(handle)) { throw_win32_error("SetConsoleActiveScreenBuffer"); }
#else
  (void)handle;
  throw std::runtime_error("The function 'terminal_set_active_screen_buffer_default' must be replaced with another function in a non-Windows environment.");
#endif
}

void terminal_set_window_width_default(const vt::Handle& handle, vt::dim_type width)
{
#if defined(_WIN32)
  /*
    * Microsoft recommended using SetConsoleScreenBufferInfoEx() but that function requires Vista or better.
    * Ansicon did it with functions available in Windows 2000.
    */
  CONSOLE_SCREEN_BUFFER_INFO csbi{};
  if(!GetConsoleScreenBufferInfo(handle, &csbi)) { throw_win32_error("GetConsoleScreenBufferInfo"); }

  auto win  = csbi.srWindow;
  win.Right = width - csbi.srWindow.Left - 1;

  auto buf_size = csbi.dwSize;
  buf_size.X    = static_cast<SHORT>(width);

  // If buffer is wider than requested window width, set new buffer size first.
  // Otherwise, set new window size first. The function calls fail if called
  // in the wrong order.
  std::vector<char> ops{'b', 'w'};
  if(width <= csbi.srWindow.Right - csbi.srWindow.Left + 1) { std::reverse(ops.begin(), ops.end()); }

  for(auto op: ops)
  {
    if(op == 'b')
    {
      if(!SetConsoleScreenBufferSize(handle, buf_size)) { throw_win32_error("SetConsoleScreenBufferSize"); }
    }
    else
    {
      if(!SetConsoleWindowInfo(handle, TRUE, &win)) { throw_win32_error("SetConsoleWindowInfo"); }
    }
  }
#else
  (void)handle;
  (void)width;
  throw std::runtime_error("The function 'terminal_set_window_width_default' must be replaced with another function in a non-Windows environment.");
#endif
}

/*
    _________________________ ______ screen_buffer.top
    |           |
    |           |
    |-----------------------|------- viewport.top
    |      static area      |
    |.......................|....... scroll_region.top
    |           |
    |     scroll region     |
    |.......................|....... scroll_region.bottom
    |      static area      |
    |-----------------------|------- viewport.bottom
    |           |
    |           |
    |           |
    |_______________________| ______ screen_buffer.bottom

  */

/**
   * @brief Apply margins to the viewport to get the scrolling region. Anything outside the
   *      scroll region (MS calls it the clipping rectangle) is fixed and will not scroll.
   * @param vp The viewport (same as CONSOLE_SCREEN_BUFFER_INFO.srWindow, but 1-based)
   * @param top_margin The 1-based line number corresponding to the top of the scroll region.
   * @param bottom_margin The 1-based line number corresponding to the bottom of the scroll region.
   * @return The scroll region.
   */
vt::rectangle console_get_scroll_region(const vt::rectangle& vp, vt::dim_type top_margin, vt::dim_type bottom_margin)
{
  vt::rectangle sw = vp;

  if(top_margin > vp.top) sw.top = top_margin;
  if(bottom_margin > vp.top && bottom_margin < vp.bottom) sw.bottom = bottom_margin;

  return sw;
}

const char* dec_special_graphics(char c)
{
  // https://github.com/microsoft/terminal
  switch(c)
  {
    case 0x5f: return "\xc2\xa0";      // Non-breaking space
    case 0x60: return "\xe2\x99\xa6";  // \u2666 Diamond (more commonly U+25C6, but U+2666 renders better for us)
    case 0x61: return "\xe2\x96\x92";  // \u2592 Checkerboard
    case 0x62: return "\xe2\x90\x89";  // \u2409 HT, SYMBOL FOR HORIZONTAL TABULATION
    case 0x63: return "\xe2\x90\x8c";  // \u240c FF, SYMBOL FOR FORM FEED
    case 0x64: return "\xe2\x90\x8d";  // \u240d CR, SYMBOL FOR CARRIAGE RETURN
    case 0x65: return "\xe2\x90\x8a";  // \u240a LF, SYMBOL FOR LINE FEED
    case 0x66: return "\xc2\xb0";      // \u00b0 Degree symbol
    case 0x67: return "\xc2\xb1";      // \u00b1 Plus/minus
    case 0x68: return "\xe2\x90\xa4";  // \u2424 NL, SYMBOL FOR NEWLINE
    case 0x69: return "\xe2\x90\x8b";  // \u240b VT, SYMBOL FOR VERTICAL TABULATION
    case 0x6a: return "\xe2\x94\x98";  // \u2518 Lower-right corner
    case 0x6b: return "\xe2\x94\x90";  // \u2510 Upper-right corner
    case 0x6c: return "\xe2\x94\x8c";  // \u250c Upper-left corner
    case 0x6d: return "\xe2\x94\x94";  // \u2514 Lower-left corner
    case 0x6e: return "\xe2\x94\xbc";  // \u253c Crossing lines
    case 0x6f: return "\xe2\x8e\xba";  // \u23ba Horizontal line - Scan 1
    case 0x70: return "\xe2\x8e\xbb";  // \u23bb Horizontal line - Scan 3
    case 0x71: return "\xe2\x94\x80";  // \u2500 Horizontal line - Scan 5
    case 0x72: return "\xe2\x8e\xbc";  // \u23bc Horizontal line - Scan 7
    case 0x73: return "\xe2\x8e\xbd";  // \u23bd Horizontal line - Scan 9
    case 0x74: return "\xe2\x94\x9c";  // \u251c Left "T"
    case 0x75: return "\xe2\x94\xa4";  // \u2524 Right "T"
    case 0x76: return "\xe2\x94\xb4";  // \u2534 Bottom "T"
    case 0x77: return "\xe2\x94\xac";  // \u252c Top "T"
    case 0x78: return "\xe2\x94\x82";  // \u2502 | Vertical bar
    case 0x79: return "\xe2\x89\xa4";  // \u2264 Less than or equal to
    case 0x7a: return "\xe2\x89\xa5";  // \u2265 Greater than or equal to
    case 0x7b: return "\xcf\x80";      // \u03c0 Pi
    case 0x7c: return "\xe2\x89\xa0";  // \u2260 Not equal to
    case 0x7d: return "\xc2\xa3";      // \u00a3 UK pound sign
    case 0x7e: return "\xc2\xb7";      // \u00b7 Centered dot
    default: return nullptr;
  }
}

std::string map_charset(const std::string& src, vt::CharSet cs)
{
  if(cs != vt::CharSet::DecSpecialGraphic) return src;

  std::string mapped(src.size() * 3, '\0');
  mapped.clear();

  for(auto c: src)
  {
    auto new_c = dec_special_graphics(c);

    if(new_c) mapped += new_c;
    else
      mapped += c;
  }

  return mapped;
}

}  // namespace

namespace Term
{
namespace Private
{
std::shared_ptr<VtEmulator> get_emulator()
{
  if(!emulator) { emulator = std::make_shared<VtEmulator>(); }
  return emulator;
}

std::string vt::csi(const char* code) { return std::string("\x1b[") + code; }

VtSequence::VtSequence(Type type, std::string content)
{
  if(content.empty()) throw std::runtime_error("VtSequence content cannot be empty.");

  auto throw_invalid = [&]()
  {
    std::stringstream what;
    what << "Sequence error - invalid content for type '" << to_char_str(type) << "' (content: \"" << content << "\")";
    throw std::runtime_error(what.str());
  };

  switch(type)
  {
    case Type::Text:
    case Type::Malformed: m_content = content; break;
    case Type::C0:
      if(content.size() != 1) throw_invalid();
      m_content = content;
      break;
    case Type::C1:
      if(content.size() != 1) throw_invalid();
      m_content = content;
      break;
    case Type::Escape:
      if(content.size() < 2 || content[0] != Char::ESC) { throw_invalid(); }

      m_content = content.substr(1);
      break;
    case Type::CSI:
      if(content.size() < 3 || content[0] != Char::ESC || content[1] != Char::CSI) { throw_invalid(); }

      m_content = content.substr(2);
      break;
    case Type::OSC:
    {
      if(content.size() < 3) throw_invalid();

      size_t start{};
      size_t end{};

      if(content[0] == Char::OSC_8bit) { start = 1; }
      else if(content[0] == Char::ESC && content[1] == Char::OSC_7bit) { start = 2; }
      else
        throw_invalid();

      end = content.size();
      if(content.back() == Char::BEL || content.back() == Char::ST) { end = content.size() - 1; }

      m_content = content.substr(start, end - start);

      if(m_content.empty()) throw_invalid();
      break;
    }
    default: throw std::runtime_error("Unknown VtSequence type " + std::to_string(static_cast<int>(type)));
  }

  m_type = type;
}

VtSequence::Type VtSequence::type() const noexcept { return m_type; }

const std::string& VtSequence::content() const noexcept { return m_content; }

std::vector<VtSequence> parse(const std::string& str)
{
  std::vector<VtSequence> result;

  if(str.empty()) return result;

  std::istringstream ss(str);
  VtParseMachine     m(ss.rdbuf(), [&result](VtSequence segment) { result.push_back(segment); });

  m.run();

  return result;
}

void parse(std::streambuf* buf, std::function<void(VtSequence)> cb)
{
  VtParseMachine m(buf, cb);
  m.run();
}

VtCommand::VtCommand(Type type) : m_cmd(type) {}

VtCommand::VtCommand(Type type, std::vector<int> params) : m_cmd(type), m_params(std::move(params)) {}

VtCommand::VtCommand(Type type, std::vector<int> params, std::vector<char> intermediates) : m_cmd(type), m_params(std::move(params)), m_intermediates(std::move(intermediates)) {}

VtCommand::VtCommand(VtSequence sequence)
{
  auto type = sequence.type();
  if(type == VtSequence::Type::CSI)
  {
    auto cmd = parse_csi(sequence);
    m_cmd    = cmd.cmd();
    if(!cmd.params().empty()) m_params = std::move(cmd.m_params);
    if(!cmd.intermediates().empty()) m_intermediates = std::move(cmd.m_intermediate);
  }
  else if(type == VtSequence::Type::Escape)
  {
    auto cmd = parse_esc(sequence);
    m_cmd    = cmd.cmd();
  }
  else if(type == VtSequence::Type::OSC)
  {
    auto cmd = parse_osc(sequence);
    m_cmd    = cmd.cmd();
    if(!cmd.params().empty()) m_params = std::move(cmd.m_params);
    if(!cmd.intermediates().empty()) m_intermediates = std::move(cmd.m_intermediate);
  }
  else { m_cmd = VtCommand::Type::Invalid; }
}

VtCommand::Type VtCommand::type() const noexcept { return m_cmd; }

const std::vector<int>& VtCommand::params() const noexcept { return m_params; }

const std::vector<char>& VtCommand::intermediates() const noexcept { return m_intermediates; }

VtEmulator::VtEmulator()
{
  m_api.handle_open                = terminal_handle_open_default;
  m_api.handle_close               = terminal_handle_close_default;
  m_api.get_state                  = terminal_get_state_default;
  m_api.scroll                     = terminal_scroll_default;
  m_api.fill                       = terminal_fill_default;
  m_api.set_cursor_position        = terminal_set_cursor_position_default;
  m_api.get_cursor_appearance      = terminal_get_cursor_appreance_default;
  m_api.set_cursor_appearance      = terminal_set_cursor_appreance_default;
  m_api.get_screen_buffer_defaults = terminal_get_screen_buffer_defaults_default;
  m_api.set_text_attributes        = terminal_set_text_attributes_default;
  m_api.write_to_input             = terminal_write_to_input_default;
  m_api.set_palette_color          = terminal_set_palette_color_default;
  m_api.write_to_output            = terminal_write_to_output_default;
  m_api.set_title                  = terminal_set_title_default;
  m_api.set_active_screen_buffer   = terminal_set_active_screen_buffer_default;
  m_api.set_window_width           = terminal_set_window_width_default;
}

VtEmulator::VtEmulator(const VtApi& api)
{
  m_api.handle_open                = api.handle_open;
  m_api.handle_close               = api.handle_close;
  m_api.get_state                  = api.get_state;
  m_api.scroll                     = api.scroll;
  m_api.fill                       = api.fill;
  m_api.set_cursor_position        = api.set_cursor_position;
  m_api.get_cursor_appearance      = api.get_cursor_appearance;
  m_api.set_cursor_appearance      = api.set_cursor_appearance;
  m_api.get_screen_buffer_defaults = api.get_screen_buffer_defaults;
  m_api.set_text_attributes        = api.set_text_attributes;
  m_api.write_to_input             = api.write_to_input;
  m_api.set_palette_color          = api.set_palette_color;
  m_api.write_to_output            = api.write_to_output;
  m_api.set_title                  = api.set_title;
  m_api.set_active_screen_buffer   = api.set_active_screen_buffer;
  m_api.set_window_width           = api.set_window_width;
}

VtEmulator::~VtEmulator()
{
  if(!m_buffers.empty() && m_active_buffer != vt::ScreenBufferType::Primary) { set_active_screen_buffer(vt::ScreenBufferType::Primary); }
}

void VtEmulator::process(const VtSequence& seq)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  switch(seq.type())
  {
    case VtSequence::Type::Text: write(seq); break;
    case VtSequence::Type::C0:
      if(seq.content()[0] != Char::NUL) write(seq);
      break;
    case VtSequence::Type::C1: write(seq); break;
    case VtSequence::Type::Malformed:
    {
      std::string err("Bad sequence received: ");
      err += seq.content();
      do_report_error(err.c_str());
      break;
    }
    case VtSequence::Type::Escape:
    case VtSequence::Type::CSI:
    case VtSequence::Type::OSC: execute(seq); break;
  }
}

void VtEmulator::write(const VtSequence& seq)
{
  try
  {
    auto& buf = get_active_screen_buffer();

    auto state = m_api.get_state(buf.handle());

    auto cursor_pos_start = state.cursor_position.row();

    // Hide the cursor while writing
    auto ca         = m_api.get_cursor_appearance(buf.handle());
    bool cursor_vis = ca.visible;
    if(cursor_vis)
    {
      ca.visible = false;
      m_api.set_cursor_appearance(buf.handle(), ca);
    }

    // Write the content
    if(buf.properties().char_set == vt::CharSet::Ascii) { m_api.write_to_output(buf.handle(), seq.content()); }
    else { m_api.write_to_output(buf.handle(), map_charset(seq.content(), buf.properties().char_set)); }

    // Refresh state
    state = m_api.get_state(buf.handle());

    // If there is a static region at the bottom of the viewport, writing began
    // above it and completed in it, scrolling is needed.
    auto sr = console_get_scroll_region(state.window, buf.properties().top_margin, buf.properties().bottom_margin);
    if(sr.bottom < state.window.bottom && cursor_pos_start <= sr.bottom && state.cursor_position.row() > sr.bottom)
    {
      int       diff = static_cast<int>(state.cursor_position.row()) - sr.bottom;
      VtCommand cmd(VtCommand::Type::ViewportScrollUp, {diff});
      if(!exec_viewport_position(cmd)) { throw std::runtime_error("Looks like handler for ViewportScrollUp command has moved"); }

      // Move cursor up too.
      auto new_col = Term::Column(static_cast<std::uint16_t>(state.cursor_position.column()));
      auto new_row = Term::Row(static_cast<std::uint16_t>(state.cursor_position.row() - diff));
      m_api.set_cursor_position(buf.handle(), Term::Cursor({new_col, new_row}));
    }

    // Restore cursor visibillity
    if(cursor_vis != ca.visible)
    {
      ca.visible = cursor_vis;
      m_api.set_cursor_appearance(buf.handle(), ca);
    }
  }
  catch(const std::exception& exp)
  {
    std::string err = exp.what();
    err += " while writing sequence: ";
    err += to_string(seq);
    do_report_error(err.c_str());
  }
}

void VtEmulator::execute(const VtSequence& seq)
{
  bool handled{false};

  VtCommand cmd(seq);

  try
  {
    switch(cmd.type())
    {
      case VtCommand::Type::Invalid:
      {
        std::string err("Received invalid command. Original sequence: ");
        err += to_string(seq);
        do_report_error(err.c_str());
        return;
      }
      case VtCommand::Type::CursorUp:
      case VtCommand::Type::CursorDown:
      case VtCommand::Type::CursorRight:
      case VtCommand::Type::CursorLeft:
      case VtCommand::Type::CursorPreviousLine:
      case VtCommand::Type::CursorNextLine:
      case VtCommand::Type::CursorHorizontalAbsolute:
      case VtCommand::Type::CursorVerticalAbsolute:
      case VtCommand::Type::CursorPositionAbsolute:
      case VtCommand::Type::CursorSave:
      case VtCommand::Type::CursorRestore: handled = exec_cursor_position(cmd); break;
      case VtCommand::Type::CursorBlink:
      case VtCommand::Type::CursorNoBlink: handled = exec_unsupported(cmd); break;
      case VtCommand::Type::CursorHide:
      case VtCommand::Type::CursorShow:
      case VtCommand::Type::CursorDefaultShape: handled = exec_cursor_visibility(cmd); break;
      case VtCommand::Type::CursorBlinkingBlock: handled = exec_unsupported(cmd); break;
      case VtCommand::Type::CursorSteadyBlock: handled = exec_cursor_visibility(cmd); break;
      case VtCommand::Type::CursorBlinkingUnderline: handled = exec_unsupported(cmd); break;
      case VtCommand::Type::CursorSteadyUnderline: handled = exec_cursor_visibility(cmd); break;
      case VtCommand::Type::CursorBlinkingBar: handled = exec_unsupported(cmd); break;
      case VtCommand::Type::CursorSteadyBar: handled = exec_cursor_visibility(cmd); break;
      case VtCommand::Type::ViewportScrollUp:
      case VtCommand::Type::ViewportScrollDown: handled = exec_viewport_position(cmd); break;
      case VtCommand::Type::TextInsertCharacter:
      case VtCommand::Type::TextDeleteCharacter:
      case VtCommand::Type::TextEraseCharacter:
      case VtCommand::Type::TextInsertLine:
      case VtCommand::Type::TextDeleteLine:
      case VtCommand::Type::TextEraseInDisplay:
      case VtCommand::Type::TextEraseInLine: handled = exec_text_modification(cmd); break;
      case VtCommand::Type::Sgr:
      case VtCommand::Type::ScreenColorPaletteSet: handled = exec_screen_format(cmd); break;
      case VtCommand::Type::ModeKeypadApplication:
      case VtCommand::Type::ModeKeypadNumeric:
      case VtCommand::Type::ModeCursorKeysApplication:
      case VtCommand::Type::ModeCursorKeysNumeric: handled = exec_unsupported(cmd); break;
      case VtCommand::Type::QueryCursorPosition:
      case VtCommand::Type::QueryDeviceAttributes: handled = exec_query(cmd); break;
      case VtCommand::Type::TabCursorSet:
      case VtCommand::Type::TabCursorForward:
      case VtCommand::Type::TabCursorBackward:
      case VtCommand::Type::TabCursorClearCurrent:
      case VtCommand::Type::TabClearAll: handled = exec_tab(cmd); break;
      case VtCommand::Type::CharsetDecLineDrawing:
      case VtCommand::Type::CharsetUsAscii: handled = exec_char_set(cmd); break;
      case VtCommand::Type::ScrollingRegionSet: handled = exec_set_property(cmd); break;
      case VtCommand::Type::WindowTitleSet: handled = exec_misc(cmd); break;
      case VtCommand::Type::ScreenBufferAlternate:
      case VtCommand::Type::ScreenBufferMain: handled = exec_select_screen_buffer(cmd); break;
      case VtCommand::Type::WindowWidth132:
      case VtCommand::Type::WindowWidth80: handled = exec_set_window_width(cmd); break;
      case VtCommand::Type::SoftReset:
      case VtCommand::Type::StringTerminator:
      case VtCommand::Type::EnableErrorReporting:
      case VtCommand::Type::DisableErrorReporting: handled = exec_misc(cmd); break;
    }

    if(!handled) { throw std::runtime_error("Unhandled command"); }
  }
  catch(const std::exception& ex)
  {
    std::string w = ex.what();
    w += " " + to_string(cmd);
    do_report_error(w.c_str());
  }
}

void VtEmulator::report_error(const char* err)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  do_report_error(err);
}

void VtEmulator::do_report_error(const char* err)
{
  if(!m_report_errors) return;

  std::stringstream ss;

  ss << "\n---ERROR---> " << err << "\n";

  write(VtSequence(VtSequence::Type::Text, ss.str()));
}

bool VtEmulator::exec_unsupported(const VtCommand& cmd)
{
  (void)cmd;
  throw std::runtime_error("Command is not supported.");
}

bool VtEmulator::exec_cursor_position(const VtCommand& cmd)
{
  auto& buf = get_active_screen_buffer();

  auto state = m_api.get_state(buf.handle());

  auto cur_cursor = state.cursor_position;

  if(cmd.type() == VtCommand::Type::CursorSave)
  {
    buf.properties().saved_cursor = cur_cursor;
    return true;
  }

  auto x = cur_cursor.column();
  auto y = cur_cursor.row();

  // Default value of 1 is used if param not supplied or is zero.
  int n{1};
  if(!cmd.params().empty() && cmd.params()[0] > 0) { n = cmd.params()[0]; }

  if(cmd.type() == VtCommand::Type::CursorUp) { y -= n; }
  else if(cmd.type() == VtCommand::Type::CursorDown) { y += n; }
  else if(cmd.type() == VtCommand::Type::CursorRight) { x += n; }
  else if(cmd.type() == VtCommand::Type::CursorLeft) { x -= n; }
  else if(cmd.type() == VtCommand::Type::CursorPreviousLine)
  {
    x = static_cast<std::size_t>(state.window.left);
    y -= n;
  }
  else if(cmd.type() == VtCommand::Type::CursorNextLine)
  {
    x = static_cast<std::size_t>(state.window.left);
    y += n;
  }
  else if(cmd.type() == VtCommand::Type::CursorHorizontalAbsolute) { x = static_cast<std::size_t>(n); }
  else if(cmd.type() == VtCommand::Type::CursorVerticalAbsolute) { y = static_cast<std::size_t>(n); }
  else if(cmd.type() == VtCommand::Type::CursorPositionAbsolute)
  {
    // Y param is first
    y = static_cast<std::size_t>(n);

    // X param is next
    x = 1;
    if(cmd.params().size() > 1 && cmd.params()[1] > 0) { x = static_cast<std::size_t>(cmd.params()[1]); }
  }
  else if(cmd.type() == VtCommand::Type::CursorRestore)
  {
    if(buf.properties().saved_cursor.empty()) { buf.properties().saved_cursor = Cursor({Column(state.window.left), Row(state.window.top)}); }

    x = buf.properties().saved_cursor.column();
    y = buf.properties().saved_cursor.row();
  }
  else { return false; }

  auto new_cursor = Cursor({Column(static_cast<vt::dim_type>(x)), Row(static_cast<vt::dim_type>(y))});

  if(new_cursor != cur_cursor)
  {
    if(new_cursor.column() > state.screen_buffer.width || new_cursor.row() > state.screen_buffer.height) { throw std::invalid_argument("Requested cursor position is outside screen buffer."); }

    m_api.set_cursor_position(buf.handle(), new_cursor);
  }

  return true;
}

bool VtEmulator::exec_cursor_visibility(const VtCommand& cmd)
{
  auto& buf = get_active_screen_buffer();

  auto ca = m_api.get_cursor_appearance(buf.handle());

  if(cmd.type() == VtCommand::Type::CursorHide) { ca.visible = false; }
  else if(cmd.type() == VtCommand::Type::CursorShow) { ca.visible = true; }
  else if(cmd.type() == VtCommand::Type::CursorDefaultShape) { ca.fill_percent = buf.defaults().cursor_fill_percent; }
  else if(cmd.type() == VtCommand::Type::CursorSteadyBlock || cmd.type() == VtCommand::Type::CursorSteadyBar) { ca.fill_percent = 100; }
  else if(cmd.type() == VtCommand::Type::CursorSteadyUnderline) { ca.fill_percent = 10; }
  else { return false; }

  m_api.set_cursor_appearance(buf.handle(), ca);

  return true;
}

bool VtEmulator::exec_viewport_position(const VtCommand& cmd)
{
  if(cmd.type() != VtCommand::Type::ViewportScrollUp && cmd.type() != VtCommand::Type::ViewportScrollDown) { return false; }

  auto& buf = get_active_screen_buffer();

  // Default value of 1 is used if param not supplied or is zero.
  int n{1};
  if(!cmd.params().empty() && cmd.params()[0] > 0) { n = cmd.params()[0]; }

  n = (cmd.type() == VtCommand::Type::ViewportScrollUp) ? -n : n;

  auto state = m_api.get_state(buf.handle());

  // Scroll region (clip rectangle) - only characters inside this region are moved, everything outside remains unchanged.
  auto sr = console_get_scroll_region(state.window, buf.properties().top_margin, buf.properties().bottom_margin);

  // Destination of the upper-left corner of the scroll rectangle
  vt::point target;
  target.x = sr.left;
  target.y = sr.top + n;

  vt::fill fill;
  fill.c                = ' ';
  fill.foreground_color = state.foreground_color;
  fill.background_color = state.background_color;

  m_api.scroll(buf.handle(), sr, sr, target, fill);

  return true;
}

bool VtEmulator::exec_text_modification(const VtCommand& cmd)
{
  auto& buf = get_active_screen_buffer();

  // Default value of 1 is used if param not supplied or is zero.
  int n{1};
  if(!cmd.params().empty() && cmd.params()[0] > 0) { n = cmd.params()[0]; }

  auto state = m_api.get_state(buf.handle());

  if(cmd.type() == VtCommand::Type::TextInsertCharacter || cmd.type() == VtCommand::Type::TextDeleteCharacter)
  {
    n = (cmd.type() == VtCommand::Type::TextInsertCharacter) ? n : -n;

    // Scroll rectangle is 1 char high and spans from current cursor position to the right side of the window.
    vt::rectangle sr;
    auto          cp = state.cursor_position;
    sr.top = sr.bottom = static_cast<vt::dim_type>(cp.row());
    sr.left            = static_cast<vt::dim_type>(cp.column());
    sr.right           = state.window.right;

    vt::point target;
    target.x = sr.left + n;
    target.y = sr.top;

    vt::fill fill;
    fill.c                = ' ';
    fill.foreground_color = state.foreground_color;
    fill.background_color = state.background_color;

    m_api.scroll(buf.handle(), sr, sr, target, fill);

    return true;
  }
  else if(cmd.type() == VtCommand::Type::TextEraseCharacter)
  {
    vt::point start{};
    start.x = static_cast<vt::dim_type>(state.cursor_position.column());
    start.y = static_cast<vt::dim_type>(state.cursor_position.row());

    vt::fill fill;
    fill.c                = ' ';
    fill.foreground_color = state.foreground_color;
    fill.background_color = state.background_color;

    m_api.fill(buf.handle(), fill, start, static_cast<std::size_t>(n));

    return true;
  }
  else if(cmd.type() == VtCommand::Type::TextInsertLine || cmd.type() == VtCommand::Type::TextDeleteLine)
  {
    auto cp = state.cursor_position;

    // If cursor is not currently within the viewport, do nothing.
    auto sw = console_get_scroll_region(state.window, buf.properties().top_margin, buf.properties().bottom_margin);
    if(cp.row() < sw.top || cp.row() > sw.bottom) { return true; }

    n = (cmd.type() == VtCommand::Type::TextInsertLine) ? n : -n;

    // Scroll rectangle is from bottom of viewport up to and including the line the cursor is on.
    sw.top = static_cast<vt::dim_type>(cp.row());

    vt::fill fill;
    fill.c                = ' ';
    fill.foreground_color = state.foreground_color;
    fill.background_color = state.background_color;

    vt::point target;
    target.x = sw.left;
    target.y = n;

    m_api.scroll(buf.handle(), sw, sw, target, fill);

    return true;
  }
  else if(cmd.type() == VtCommand::Type::TextEraseInDisplay)
  {
    // default param value for this method is zero
    n = cmd.params().empty() ? 0 : static_cast<vt::dim_type>(cmd.params()[0]);

    auto        width = state.screen_buffer.width;
    std::size_t len{};

    vt::point start{};
    start.x = state.window.left;
    start.y = state.window.top;

    // scrolling margins are ignored for this method.
    if(n == 1)
    {  // Erases from the beginning of the display up to and including the current cursor position.
      len = (state.cursor_position.row() - start.y) * width;
      len += state.cursor_position.column() + 1;
    }
    else if(n == 2)
    {  // Erase the entire display.
      len = std::size_t(state.window.bottom - state.window.top) * width;
    }
    else if(n == 3)
    {  // Erase the entire display and scroll-back buffer
      len = static_cast<std::size_t>(state.screen_buffer.height) * width;
    }
    else
    {  // Erase from the current cursor position (inclusive) to the end of the display.
      len = (state.window.bottom - state.cursor_position.row()) * width;
      len += width - state.cursor_position.column();

      start.x = static_cast<vt::dim_type>(state.cursor_position.column());
      start.y = static_cast<vt::dim_type>(state.cursor_position.row());
    }

    vt::fill fill;
    fill.c                = ' ';
    fill.foreground_color = state.foreground_color;
    fill.background_color = state.background_color;

    m_api.fill(buf.handle(), fill, start, len);

    return true;
  }
  else if(cmd.type() == VtCommand::Type::TextEraseInLine)
  {
    // default param value for this method is zero
    n = cmd.params().empty() ? 0 : static_cast<vt::dim_type>(cmd.params()[0]);

    auto        width = state.screen_buffer.width;
    std::size_t len{};

    vt::point start{};
    start.x = static_cast<vt::dim_type>(state.cursor_position.column());
    start.y = static_cast<vt::dim_type>(state.cursor_position.row());

    // scrolling margins are ignored for this method.
    if(n == 1)
    {  // Erases from the beginning of the line up to and including the current cursor position.
      len     = state.cursor_position.column() + 1;
      start.x = state.window.left;
    }
    else if(n == 2)
    {  // Erase the entire line.
      len     = width;
      start.x = state.window.left;
    }
    else
    {  // Erase from the current cursor position (inclusive) to the end of the line.
      len = width - state.cursor_position.column() + 1;
    }

    vt::fill fill;
    fill.c                = ' ';
    fill.foreground_color = state.foreground_color;
    fill.background_color = state.background_color;

    m_api.fill(buf.handle(), fill, start, len);

    return true;
  }
  else { return false; }
}

bool VtEmulator::exec_screen_format(const VtCommand& cmd)
{
  auto& buf = get_active_screen_buffer();

  if(cmd.type() == VtCommand::Type::ScreenColorPaletteSet)
  {
    if(cmd.params().size() < 4) { throw std::runtime_error("Required parameters are missing."); }

    if(cmd.params()[0] < 0 || cmd.params()[0] > 15) { throw std::invalid_argument("Color palette index is out of range."); }

    for(std::size_t i = 1; i < 4; ++i)
    {
      if(cmd.params()[i] < 0 || cmd.params()[i] > 255) { throw std::invalid_argument("RGB value is out of range."); }
    }

    vt::rgb rgb;
    rgb.r = static_cast<vt::tiny_type>(cmd.params()[1]);
    rgb.g = static_cast<vt::tiny_type>(cmd.params()[2]);
    rgb.b = static_cast<vt::tiny_type>(cmd.params()[3]);

    m_api.set_palette_color(buf.handle(), static_cast<vt::tiny_type>(cmd.params()[0]), rgb);

    return true;
  }
  else if(cmd.type() == VtCommand::Type::Sgr)
  {
    // If no params, assign default
    std::vector<int> default_params{0};
    auto&            params = cmd.params().empty() ? default_params : cmd.params();

    select_graphic_rendition sgr_mod = buf.properties().sgr;

    for(size_t i = 0; i < params.size(); ++i)
    {
      switch(params[i])
      {
        case 0:
          sgr_mod.foreground_color = buf.defaults().sgr.foreground_color;
          sgr_mod.background_color = buf.defaults().sgr.background_color;
          sgr_mod.bold             = false;
          sgr_mod.underline        = false;
          sgr_mod.reversed         = false;
          break;
        case 1: sgr_mod.bold = true; break;
        case 4: sgr_mod.underline = true; break;
        case 7: sgr_mod.reversed = true; break;
        case 22: sgr_mod.bold = false; break;
        case 24: sgr_mod.underline = false; break;
        case 27: sgr_mod.reversed = false; break;
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37: sgr_mod.foreground_color = static_cast<vt::tiny_type>(params[i]); break;
        case 39: sgr_mod.foreground_color = buf.defaults().sgr.foreground_color; break;
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47: sgr_mod.background_color = static_cast<vt::tiny_type>(params[i]); break;
        case 49: sgr_mod.background_color = buf.defaults().sgr.background_color; break;
        case 90:
        case 91:
        case 92:
        case 93:
        case 94:
        case 95:
        case 96:
        case 97: sgr_mod.foreground_color = static_cast<vt::tiny_type>(params[i]); break;
        case 100:
        case 101:
        case 102:
        case 103:
        case 104:
        case 105:
        case 106:
        case 107: sgr_mod.background_color = static_cast<vt::tiny_type>(params[i]); break;
        case 38:
        case 48:
        {
          auto throw_invalid_sgr_subseq = []() { throw std::invalid_argument("Invalid SGR subsequence"); };

          bool  fg = params[i] == 38 ? true : false;
          Color color;

          if(i + 1 >= params.size()) throw_invalid_sgr_subseq();

          switch(params[i + 1])
          {
            case 2:
            {
              if(i + 4 >= params.size()) throw_invalid_sgr_subseq();

              std::vector<std::uint8_t> rgb;

              ++i;
              while(rgb.size() < 3)
              {
                ++i;
                if(params[i] < 0 || params[i] > 255) { throw std::invalid_argument("Invalid RGB value"); }
                rgb.push_back(static_cast<vt::tiny_type>(params[i]));
              }

              color = Color(rgb[0], rgb[1], rgb[2]);

              break;
            }
            case 5:
            {
              if(i + 2 >= params.size()) throw_invalid_sgr_subseq();

              i += 2;

              color = Color(static_cast<vt::tiny_type>(params[i]));
              break;
            }
            default: throw_invalid_sgr_subseq();
          }

          if(fg) { sgr_mod.foreground_color = static_cast<vt::tiny_type>(static_cast<int>(color.to4bits()) + 30); }
          else { sgr_mod.background_color = static_cast<vt::tiny_type>(static_cast<int>(color.to4bits()) + 40); }

          break;
        }
      }
    }

    if(sgr_mod != buf.properties().sgr)
    {
      vt::text_attributes attribs;
      auto                fg = sgr_mod.foreground_color;
      auto                bg = sgr_mod.background_color;

      if(sgr_mod.bold && (fg >= 30 && fg <= 37)) fg += 60;
      if(sgr_mod.underline && (bg >= 40 && bg <= 47)) bg += 60;

      if(!sgr_mod.reversed)
      {
        attribs.foreground_color = fg;
        attribs.background_color = bg;
      }
      else
      {
        attribs.foreground_color = static_cast<vt::tiny_type>(bg - 10);
        attribs.background_color = static_cast<vt::tiny_type>(fg + 10);
      }

      m_api.set_text_attributes(buf.handle(), attribs);

      buf.properties().sgr = sgr_mod;
    }

    return true;
  }

  return false;
}

bool VtEmulator::exec_query(const VtCommand& cmd)
{
  auto& buf = get_active_screen_buffer();

  std::string response;

  if(cmd.type() == VtCommand::Type::QueryCursorPosition)
  {
    auto state  = m_api.get_state(buf.handle());
    auto cursor = state.cursor_position;
    auto pos    = std::to_string(cursor.row()) + ";" + std::to_string(cursor.column()) + "R";
    response    = vt::csi(pos.c_str());
  }
  else if(cmd.type() == VtCommand::Type::QueryDeviceAttributes)
  {
    response = vt::csi("?1;0c");  // VT101 with No Options
  }
  else { return false; }

  auto handle = m_api.handle_open(VtApi::TerminalHandleType::StdIn, m_api.handle_close);
  m_api.write_to_input(handle, response);

  return true;
}

bool VtEmulator::exec_tab(const VtCommand& cmd)
{
  auto& buf = get_active_screen_buffer();

  using set_t = decltype(buf.properties().tabs)::key_type;
  using cur_t = std::uint16_t;

  if(cmd.type() == VtCommand::Type::TabClearAll)
  {
    buf.properties().tabs.clear();
    return true;
  }

  auto state      = m_api.get_state(buf.handle());
  auto cur_cursor = state.cursor_position;

  // Default value of 1 is used if param not supplied or is zero.
  int n{1};
  if(!cmd.params().empty() && cmd.params()[0] > 0) { n = cmd.params()[0]; }

  if(cmd.type() == VtCommand::Type::TabCursorSet)
  {
    buf.properties().tabs.insert(static_cast<set_t>(cur_cursor.column()));
    return true;
  }
  else if(cmd.type() == VtCommand::Type::TabCursorForward || cmd.type() == VtCommand::Type::TabCursorBackward)
  {
    /*
        * Microsoft documentation states "If there are no tab stops set via HTS, CHT and CBT
        * will treat the first and last columns of the window as the only two tab stops."
        * https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#tabs
        *
        * For forward movement "If there are no more tab stops, move to the last column in the
        * row. If the cursor is in the last column, move to the first column of the next row."
        *
        * For backward movement "If there are no more tab stops, moves the cursor to the first
        * column. If the cursor is in the first column, doesn’t move the cursor."
        *
        * So, in effect, there are always tabs at the first and last columns.
        */
    decltype(buf.properties().tabs) tabs_effective = buf.properties().tabs;
    tabs_effective.insert(static_cast<set_t>(state.window.left));
    tabs_effective.insert(static_cast<set_t>(state.window.right));

    auto cur_col = static_cast<set_t>(cur_cursor.column());

    Term::Column new_col(*tabs_effective.begin());
    Term::Row    new_row(static_cast<cur_t>(cur_cursor.row()));

    if(cmd.type() == VtCommand::Type::TabCursorForward)
    {
      auto it = tabs_effective.begin();
      for(; it != tabs_effective.end(); it++)
      {
        if(*it > cur_col && --n < 1) break;
      }

      if(it != tabs_effective.end()) { new_col = Term::Column(*it); }
      else { new_row = Term::Row(static_cast<cur_t>(cur_cursor.row() + 1)); }
    }
    else
    {
      auto it = tabs_effective.rbegin();
      for(; it != tabs_effective.rend(); ++it)
      {
        if(*it < cur_col && --n < 1) break;
      }

      if(it != tabs_effective.rend()) { new_col = Term::Column(*it); }
    }

    auto new_cursor = Term::Cursor({new_col, new_row});

    if(new_cursor != cur_cursor) { m_api.set_cursor_position(buf.handle(), new_cursor); }

    return true;
  }
  else if(cmd.type() == VtCommand::Type::TabCursorClearCurrent)
  {
    auto& tabs = buf.properties().tabs;
    auto  it   = tabs.find(static_cast<set_t>(cur_cursor.column()));
    if(it != tabs.end()) tabs.erase(it);
    return true;
  }
  else { return false; }
}

bool VtEmulator::exec_set_property(const VtCommand& cmd)
{
  auto& buf = get_active_screen_buffer();

  if(cmd.type() == VtCommand::Type::ScrollingRegionSet)
  {
    auto  state = m_api.get_state(buf.handle());
    auto& win   = state.window;

    // Default value of 1 is used if param not supplied or is zero.
    vt::dim_type top{1};
    if(!cmd.params().empty() && cmd.params()[0] > 0) { top = static_cast<vt::dim_type>(cmd.params()[0]); }

    // Default is the current viewport height.
    auto bottom = win.bottom;
    if(cmd.params().size() > 1 && cmd.params()[1] > 0) { bottom = static_cast<vt::dim_type>(cmd.params()[1]); }

    if((top < win.top || top > win.bottom) && (bottom < win.top || bottom > win.bottom)) { throw std::out_of_range("Specified margins are outside of viewport"); }
    else if(top < win.top || top > win.bottom) { throw std::out_of_range("Top margin is outside of viewport"); }
    else if(bottom < win.top || bottom > win.bottom) { throw std::out_of_range("Bottom margin is outside of viewport"); }
    else if(top >= bottom) { throw std::out_of_range("Top margin must be less than bottom"); }

    buf.properties().top_margin    = top;
    buf.properties().bottom_margin = bottom;

    // DECSTBM moves the cursor to column 1, line 1 of the page. (https://vt100.net/docs/vt510-rm/DECSTBM.html)
    if(state.cursor_position.column() != win.left || state.cursor_position.row() != win.top) { m_api.set_cursor_position(buf.handle(), Cursor({Column(win.left), Row(win.top)})); }
    return true;
  }
  else { return false; }
}

bool VtEmulator::exec_char_set(const VtCommand& cmd)
{
  auto& buf = get_active_screen_buffer();

  if(cmd.type() == VtCommand::Type::CharsetDecLineDrawing)
  {
    buf.properties().char_set = vt::CharSet::DecSpecialGraphic;
    return true;
  }
  else if(cmd.type() == VtCommand::Type::CharsetUsAscii)
  {
    buf.properties().char_set = vt::CharSet::Ascii;
    return true;
  }

  return false;
}

bool VtEmulator::exec_select_screen_buffer(const VtCommand& cmd)
{
  if(cmd.type() == VtCommand::Type::ScreenBufferAlternate)
  {
    set_active_screen_buffer(vt::ScreenBufferType::Alternate);
    return true;
  }
  else if(cmd.type() == VtCommand::Type::ScreenBufferMain)
  {
    set_active_screen_buffer(vt::ScreenBufferType::Primary);
    return true;
  }

  return false;
}

bool VtEmulator::exec_set_window_width(const VtCommand& cmd)
{
  vt::dim_type width{};

  if(cmd.type() == VtCommand::Type::WindowWidth132) { width = 132; }
  else if(cmd.type() == VtCommand::Type::WindowWidth80) { width = 80; }
  else { return false; }

  auto& buf = get_active_screen_buffer();
  m_api.set_window_width(buf.handle(), width);

  /*
      * If you change the DECCOLM setting, the terminal:
      * - Sets the left, right, top and bottom scrolling margins to their default positions.
      * - Erases all data in page memory.
      *
      * http://vt100.net/docs/vt510-rm/DECCOLM
      */
  std::vector<VtSequence> reset_ops;
  reset_ops.push_back(VtSequence(VtSequence::Type::CSI, "r"));   // default margins
  reset_ops.push_back(VtSequence(VtSequence::Type::CSI, "3J"));  // erase data in display and scroll-back memory

  for(auto& op: reset_ops) { execute(op); }

  return true;
}

bool VtEmulator::exec_misc(const VtCommand& cmd)
{
  if(cmd.type() == VtCommand::Type::WindowTitleSet)
  {
    auto& buf = get_active_screen_buffer();

    std::string title;
    for(auto c: cmd.intermediates()) { title += c; }

    m_api.set_title(buf.handle(), title);
    return true;
  }
  else if(cmd.type() == VtCommand::Type::SoftReset)
  {
    auto& buf   = get_active_screen_buffer();
    auto  state = m_api.get_state(buf.handle());

    buf.properties().saved_cursor = Cursor({Column(state.window.left), Row(state.window.top)});

    std::vector<VtSequence> reset_ops;
    reset_ops.push_back(VtSequence(VtSequence::Type::CSI, vt::csi("?25h")));  // show cursor
    reset_ops.push_back(VtSequence(VtSequence::Type::CSI, vt::csi("r")));     // default margins
    reset_ops.push_back(VtSequence(VtSequence::Type::Escape, "\x1b(B"));      // ASCII character set
    reset_ops.push_back(VtSequence(VtSequence::Type::CSI, vt::csi("m")));     // default SGR

    for(auto& op: reset_ops) { execute(op); }

    return true;
  }
  else if(cmd.type() == VtCommand::Type::StringTerminator)
  {
    // no-op control function per Paul Flo William's state diagram.
    return true;
  }
  else if(cmd.type() == VtCommand::Type::EnableErrorReporting)
  {
    m_report_errors = true;
    return true;
  }
  else if(cmd.type() == VtCommand::Type::DisableErrorReporting)
  {
    m_report_errors = false;
    return true;
  }
  else { return false; }
}

VtEmulator::ScreenBuffer& VtEmulator::get_active_screen_buffer() { return get_screen_buffer(m_active_buffer); }

VtEmulator::ScreenBuffer& VtEmulator::get_primary_screen_buffer() { return get_screen_buffer(vt::ScreenBufferType::Primary); }

VtEmulator::ScreenBuffer& VtEmulator::get_alternate_screen_buffer() { return get_screen_buffer(vt::ScreenBufferType::Alternate); }

VtEmulator::ScreenBuffer& VtEmulator::get_screen_buffer(vt::ScreenBufferType type)
{
  if(m_buffers.empty())
  {
    auto handle   = m_api.handle_open(VtApi::TerminalHandleType::OutPrimary, m_api.handle_close);
    auto defaults = m_api.get_screen_buffer_defaults(handle);
    m_buffers.emplace_back(std::move(handle), defaults);
  }

  if(type == vt::ScreenBufferType::Primary) { return m_buffers[0]; }
  else if(type == vt::ScreenBufferType::Alternate)
  {
    if(m_buffers.size() < 2)
    {
      auto handle   = m_api.handle_open(VtApi::TerminalHandleType::OutAlternate, m_api.handle_close);
      auto defaults = m_api.get_screen_buffer_defaults(handle);
      m_buffers.emplace_back(std::move(handle), defaults);
    }

    return m_buffers[1];
  }
  else { throw std::runtime_error("Invalid screen buffer requested"); }
}

void VtEmulator::set_active_screen_buffer(vt::ScreenBufferType type)
{
  auto& buf = get_screen_buffer(type);
  m_api.set_active_screen_buffer(buf.handle());
  m_active_buffer = type;
}

bool VtEmulator::select_graphic_rendition::operator==(const select_graphic_rendition& rhs) const
{
  if(this->foreground_color == rhs.foreground_color && this->background_color == rhs.background_color && this->bold == rhs.bold && this->underline == rhs.underline && this->reversed == rhs.reversed) { return true; }

  return false;
}

bool VtEmulator::select_graphic_rendition::operator!=(const select_graphic_rendition& rhs) const { return !(*this == rhs); }

vt::Handle::Handle(raw_handle_type handle, std::function<void(raw_handle_type)> handle_closer_fn) : m_handle(handle), m_closer(handle_closer_fn) {}

vt::Handle::~Handle() { m_closer(m_handle); }

vt::Handle::Handle(Handle&& other) noexcept { *this = std::move(other); }

vt::Handle& vt::Handle::operator=(Handle&& other) noexcept
{
  if(this != &other)
  {
    m_handle = other.m_handle;
    m_closer = other.m_closer;

    other.m_handle = nullptr;
  }

  return *this;
}

VtEmulator::ScreenBuffer::ScreenBuffer(vt::Handle handle, const vt::screen_buffer_defaults& defaults) : m_handle(std::move(handle))
{
  m_defaults.cursor_fill_percent  = defaults.cursor_fill_percent;
  m_defaults.sgr.foreground_color = defaults.foreground_color;
  m_defaults.sgr.background_color = defaults.background_color;
  m_props.sgr                     = m_defaults.sgr;
}

const vt::Handle& VtEmulator::ScreenBuffer::handle() const noexcept { return m_handle; }

VtEmulator::buffer_defaults& VtEmulator::ScreenBuffer::defaults() noexcept { return m_defaults; }

VtEmulator::buffer_properties& VtEmulator::ScreenBuffer::properties() noexcept { return m_props; }

VtWriter::VtWriter() : m_emulator(get_emulator()) {}

std::size_t VtWriter::write(const std::string& str)
{
  if(!str.empty())
  {
    m_stream << str;
    do_write();
  }

  return str.size();
}

std::size_t VtWriter::write(const char& c)
{
  m_stream << c;
  do_write();
  return 1;
}

void VtWriter::handle_sequence(VtSequence seq)
{
  try
  {
    m_emulator->process(seq);
  }
  catch(const std::exception& exp)
  {
    report_error(exp.what());
  }
}

void VtWriter::do_write()
{
  try
  {
    parse(m_stream.rdbuf(), [this](VtSequence seq) { handle_sequence(std::move(seq)); });
  }
  catch(const std::exception& exp)
  {
    report_error(exp.what());
  }

  if(m_stream.peek() == std::stringstream::traits_type::eof())
  {
    m_stream.str(std::string());
    m_stream.clear();
  }
}
void VtWriter::report_error(const char* err) { m_emulator->report_error(err); }
}  // namespace Private
}  // namespace Term
