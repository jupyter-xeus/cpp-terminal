/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2026 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once
#include "cpp-terminal/cursor.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <ostream>
#include <set>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

namespace Term
{
namespace Private
{
namespace vt
{
using dim_type  = std::uint16_t;
using tiny_type = std::uint8_t;

enum class CharSet
{
  Ascii,
  DecSpecialGraphic
};
enum class ScreenBufferType
{
  Primary,
  Alternate
};

struct point
{
  int x{};
  int y{};
};

struct dimension
{
  dim_type height{};
  dim_type width{};
  dim_type depth{};
};

struct rectangle
{
  dim_type top{};
  dim_type right{};
  dim_type bottom{};
  dim_type left{};
};

struct fill
{
  char      c{};
  tiny_type foreground_color{};
  tiny_type background_color{};
};

struct cursor_appearance
{
  tiny_type fill_percent{};
  bool      visible{};
};

struct screen_buffer_defaults
{
  tiny_type cursor_fill_percent{};
  tiny_type foreground_color{};
  tiny_type background_color{};
};

struct text_attributes
{
  tiny_type foreground_color{};
  tiny_type background_color{};
};

struct rgb
{
  tiny_type r{};
  tiny_type g{};
  tiny_type b{};
};

class Handle
{
public:
  using raw_handle_type = void*;

  Handle(raw_handle_type handle, std::function<void(raw_handle_type)> closer);
  ~Handle();
  Handle(const Handle& rhs)            = delete;
  Handle& operator=(const Handle& rhs) = delete;
  Handle(Handle&& other) noexcept;
  Handle& operator=(Handle&& other) noexcept;

  operator raw_handle_type() const noexcept { return m_handle; }

private:
  raw_handle_type                      m_handle{nullptr};
  std::function<void(raw_handle_type)> m_closer;
};

struct state
{
  vt::dimension screen_buffer;
  Cursor        cursor_position;
  vt::tiny_type foreground_color{};
  vt::tiny_type background_color{};
  vt::rectangle window;
};

std::string csi(const char* code);
}  // namespace vt

class VtSequence
{
public:
  enum class Type
  {
    Text,
    Malformed,
    C0,
    C1,
    Escape,
    CSI,  // Control Sequence Introducer
    OSC   // Operating System Command
  };

  VtSequence(Type type, std::string content);

  Type type() const noexcept;

  const std::string& content() const noexcept;

private:
  Type        m_type;
  std::string m_content;
};

std::vector<VtSequence> parse(const std::string& str);
void                    parse(std::streambuf* buf, std::function<void(VtSequence)> cb);

class VtCommand
{
public:
  enum class Type
  {
    Invalid,     // The data provided does not represent a valid command.
    CursorUp,    // No column change.
    CursorDown,  // No column change.
    CursorRight,
    CursorLeft,
    CursorPreviousLine,        // Same as CursorUp, but column is set to first position.
    CursorNextLine,            // Same as CursorDown, but column is set to first position.
    CursorHorizontalAbsolute,  // Move cursor to specific column, no line change.
    CursorVerticalAbsolute,    // Move cursor to specific line, no column change.
    CursorPositionAbsolute,    // Move cursor to specific line and column.
    CursorSave,                // Save cursor position.
    CursorRestore,             // Restore cursor to last saved position.
    CursorBlink,
    CursorNoBlink,
    CursorHide,
    CursorShow,
    CursorDefaultShape,         // Default cursor shape of the terminal.
    CursorBlinkingBlock,        // Blinking block cursor shape.
    CursorSteadyBlock,          // Steady block cursor shape.
    CursorBlinkingUnderline,    // Blinking underline cursor shape.
    CursorSteadyUnderline,      // Steady underline cursor shape.
    CursorBlinkingBar,          // Blinking bar cursor shape.
    CursorSteadyBar,            // Steady bar cursor shape.
    ViewportScrollUp,           // Scroll text up. Also known as pan down, new lines fill in from the bottom of the screen.
    ViewportScrollDown,         // Scroll text down. Also known as pan up, new lines fill in from the top of the screen.
    TextInsertCharacter,        // Insert spaces at the current cursor position, shifting all existing text to the right. Text exiting the screen to the right is removed.
    TextDeleteCharacter,        // Delete characters at the current cursor position, shifting in space characters from the right edge of the screen.
    TextEraseCharacter,         // Erase characters from the current cursor position by overwriting them with a space character.
    TextInsertLine,             // Inserts lines into the buffer at the cursor position.The line the cursor is on, and lines below it, will be shifted downwards.
    TextDeleteLine,             // Deletes lines from the buffer, starting with the row the cursor is on.
    TextEraseInDisplay,         // Replace all text in the current viewport / screen specified by <n> with space characters. Works inside or outside the scrolling margins.
    TextEraseInLine,            // Replace all text on the line with the cursor specified by <n> with space characters.
    Sgr,                        // Select Graphic Rendition
    ScreenColorPaletteSet,      // Sets a screen color palette index to the RGB values specified.
    ModeKeypadApplication,      // Keypad keys will emit their Application Mode sequences.
    ModeKeypadNumeric,          // Keypad keys will emit their Numeric Mode sequences.
    ModeCursorKeysApplication,  // Keypad keys will emit their Application Mode sequences.
    ModeCursorKeysNumeric,      // Keypad keys will emit their Numeric Mode sequences.
    QueryCursorPosition,        // Emit the cursor position as : ESC[<r>; <c> R Where <r> = cursor row and <c> = cursor column
    QueryDeviceAttributes,      // Report the terminal identity.Will emit “\x1b[? 1; 0c”, indicating "VT101 with No Options".
    TabCursorSet,               // Sets a tab stop in the current column the cursor is in.
    TabCursorForward,           // Advance the cursor to the next column(in the same row) with a tab stop.If there are no more tab stops, move to the last column in the row.If the cursor is in the last column, move to the first column of the next row.
    TabCursorBackward,          // Move the cursor to the previous column(in the same row) with a tab stop.If there are no more tab stops, moves the cursor to the first column.If the cursor is in the first column, doesn’t move the cursor.
    TabCursorClearCurrent,      // Clears the tab stop in the current column, if there is one.Otherwise does nothing.
    TabClearAll,                // Clears all currently set tab stops.
    CharsetDecLineDrawing,      // Enables DEC Line Drawing Mode.
    CharsetUsAscii,             // Enables ASCII Mode.
    ScrollingRegionSet,         // Sets the VT scrolling margins of the viewport.
    WindowTitleSet,             // Sets the console window’s title
    ScreenBufferAlternate,      // Switches to a new alternate screen buffer.
    ScreenBufferMain,           // Use the main main buffer.
    WindowWidth132,             // Sets the console width to 132 columns wide.
    WindowWidth80,              // Sets the console width to 80 columns wide.
    SoftReset,                  // Reset certain terminal settings to their defaults.
    StringTerminator,           // String Terminator (ST) control function used in 7 bit environments to terminate DCS, OSC, PM or APC control strings
    // CPP-Terminal exclusives
    EnableErrorReporting,   // Display error message when parsing or command execution fails.
    DisableErrorReporting,  // Display error message when parsing or command execution fails.
  };

  VtCommand() = default;
  VtCommand(Type type);
  VtCommand(Type type, std::vector<int> params);
  VtCommand(Type type, std::vector<int> params, std::vector<char> intermediates);
  VtCommand(VtSequence sequence);

  Type                     type() const noexcept;
  const std::vector<int>&  params() const noexcept;
  const std::vector<char>& intermediates() const noexcept;

private:
  Type              m_cmd{VtCommand::Type::Invalid};
  std::vector<int>  m_params;
  std::vector<char> m_intermediates;
};

struct VtApi
{
  enum class TerminalHandleType
  {
    OutPrimary,
    OutAlternate,
    StdIn
  };

  std::function<vt::Handle(TerminalHandleType, std::function<void(vt::Handle::raw_handle_type)> closer)>                                             handle_open;
  std::function<void(vt::Handle::raw_handle_type)>                                                                                                   handle_close;
  std::function<vt::state(const vt::Handle&)>                                                                                                        get_state;
  std::function<void(const vt::Handle&, const vt::rectangle& scroll, const vt::rectangle& clip, const vt::point& destination, const vt::fill& fill)> scroll;
  std::function<void(const vt::Handle&, const vt::fill& fill, const vt::point& at, std::size_t len)>                                                 fill;
  std::function<void(const vt::Handle&, const Term::Cursor& cursor)>                                                                                 set_cursor_position;
  std::function<vt::cursor_appearance(const vt::Handle&)>                                                                                            get_cursor_appearance;
  std::function<void(const vt::Handle&, const vt::cursor_appearance& ca)>                                                                            set_cursor_appearance;
  std::function<vt::screen_buffer_defaults(const vt::Handle&)>                                                                                       get_screen_buffer_defaults;
  std::function<void(const vt::Handle&, const vt::text_attributes& attribs)>                                                                         set_text_attributes;
  std::function<void(const vt::Handle&, const std::string& str)>                                                                                     write_to_input;
  std::function<void(const vt::Handle&, vt::tiny_type index, const vt::rgb& rgb)>                                                                    set_palette_color;
  std::function<void(const vt::Handle&, const std::string& str)>                                                                                     write_to_output;
  std::function<void(const vt::Handle&, const std::string& str)>                                                                                     set_title;
  std::function<void(const vt::Handle&)>                                                                                                             set_active_screen_buffer;
  std::function<void(const vt::Handle&, vt::dim_type width)>                                                                                         set_window_width;
};

/**
 * @brief Calls windows console API functions to emulate a virtual terminal.
 * 
 * @remarks This class maintains certain state properties, such as scrolling window margins.
 *			Therefore, it is important that only one instance is in use at a time for 
 *			proper operation.
 */
class VtEmulator
{
public:
  VtEmulator();
  VtEmulator(const VtApi& api);
  ~VtEmulator();
  VtEmulator(const VtEmulator& rhs)            = delete;
  VtEmulator& operator=(const VtEmulator& rhs) = delete;
  VtEmulator(VtEmulator&& other)               = delete;
  VtEmulator& operator=(VtEmulator&& other)    = delete;

  void process(const VtSequence& seq);
  void report_error(const char* err);

private:
  struct sgr
  {
    vt::tiny_type foreground_color{0};
    vt::tiny_type background_color{0};
    bool          bold{false};       // If Win can't do bold, a bright version of the foreground color is used.
    bool          underline{false};  // Win can't do underline, so a bright version of the background color is used.
    bool          reversed{false};   // foreground and background colors are reversed

    bool operator==(const sgr& rhs) const;
    bool operator!=(const sgr& rhs) const;
  };

  struct buffer_defaults
  {
    vt::tiny_type cursor_fill_percent{};
    sgr           sgr;
  };

  struct buffer_properties
  {
    sgr                     sgr;
    std::set<std::uint16_t> tabs;
    vt::dim_type            top_margin{};     // 1-based row number representing the top of the scrolling window.
    vt::dim_type            bottom_margin{};  // 1-based row number representing the bottom of the scrolling window.
    vt::CharSet             char_set{vt::CharSet::Ascii};
    Cursor                  saved_cursor;
  };

  class ScreenBuffer
  {
  public:
    ScreenBuffer()  = delete;
    ~ScreenBuffer() = default;
    ScreenBuffer(vt::Handle handle, const vt::screen_buffer_defaults& defaults);
    ScreenBuffer(const ScreenBuffer& rhs)            = delete;
    ScreenBuffer& operator=(const ScreenBuffer& rhs) = delete;
    ScreenBuffer(ScreenBuffer&& other)               = default;
    ScreenBuffer& operator=(ScreenBuffer&& other)    = default;

    const vt::Handle&  handle() const noexcept;
    buffer_defaults&   defaults() noexcept;
    buffer_properties& properties() noexcept;

  private:
    vt::Handle        m_handle;
    buffer_defaults   m_defaults;
    buffer_properties m_props;
  };

  ScreenBuffer& get_active_screen_buffer();
  ScreenBuffer& get_primary_screen_buffer();
  ScreenBuffer& get_alternate_screen_buffer();
  ScreenBuffer& get_screen_buffer(vt::ScreenBufferType type);

  void set_active_screen_buffer(vt::ScreenBufferType type);

  void write(const VtSequence& seq);
  void execute(const VtSequence& seq);
  void do_report_error(const char* err);

  [[nodiscard]] bool exec_unsupported(const VtCommand& cmd);
  [[nodiscard]] bool exec_cursor_position(const VtCommand& cmd);
  [[nodiscard]] bool exec_cursor_visibility(const VtCommand& cmd);
  [[nodiscard]] bool exec_viewport_position(const VtCommand& cmd);
  [[nodiscard]] bool exec_text_modification(const VtCommand& cmd);
  [[nodiscard]] bool exec_screen_format(const VtCommand& cmd);
  [[nodiscard]] bool exec_query(const VtCommand& cmd);
  [[nodiscard]] bool exec_tab(const VtCommand& cmd);
  [[nodiscard]] bool exec_set_property(const VtCommand& cmd);
  [[nodiscard]] bool exec_char_set(const VtCommand& cmd);
  [[nodiscard]] bool exec_select_screen_buffer(const VtCommand& cmd);
  [[nodiscard]] bool exec_set_window_width(const VtCommand& cmd);
  [[nodiscard]] bool exec_misc(const VtCommand& cmd);

  mutable std::mutex        m_mutex;
  std::vector<ScreenBuffer> m_buffers;
  vt::ScreenBufferType      m_active_buffer{vt::ScreenBufferType::Primary};
  VtApi                     m_api;
  bool                      m_report_errors{false};
};

std::shared_ptr<VtEmulator> get_emulator();

class VtWriter
{
public:
  VtWriter();
  virtual ~VtWriter()                      = default;
  VtWriter(const VtWriter& rhs)            = delete;
  VtWriter& operator=(const VtWriter& rhs) = delete;
  VtWriter(VtWriter&& other)               = delete;
  VtWriter& operator=(VtWriter&& other)    = delete;

  std::size_t write(const std::string& str);
  std::size_t write(const char& c);

protected:
  virtual void handle_sequence(VtSequence seq);

private:
  void do_write();
  void report_error(const char* err);

  std::shared_ptr<VtEmulator> m_emulator;
  std::stringstream m_stream;
};

}  // namespace Private
}  // namespace Term