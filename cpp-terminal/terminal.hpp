#pragma once

namespace Term
{

/* Note: the code that uses Terminal must be inside try/catch block, otherwise
 * the destructors will not be called when an exception happens and the
 * terminal will not be left in a good state. Terminal uses exceptions when
 * something goes wrong.
 */
class Terminal
{
public:
  Terminal(bool _clear_screen, bool enable_keyboard, bool disable_signal_keys, bool _hide_cursor);
  // providing no parameters will disable the keyboard and ctrl+c
  explicit Terminal(bool _clear_screen);
  virtual ~Terminal();

private:
  void store_and_restore();
  bool clear_screen{};
  bool keyboard_enabled{false};
  bool disable_signal_keys{true};
  bool hide_cursor{};
};

}  // namespace Term
