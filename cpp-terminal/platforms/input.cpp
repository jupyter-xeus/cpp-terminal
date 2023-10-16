/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#if defined(_WIN32)
  #include "cpp-terminal/platforms/unicode.hpp"

  #include <vector>
  #include <windows.h>
#elif defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
  #include <cerrno>
  #include <csignal>
  #include <sys/ioctl.h>
  #include <thread>
  #include <unistd.h>
#else
  #include <memory>
  #include <sys/epoll.h>
#endif

#include "cpp-terminal/event.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/platforms/blocking_queue.hpp"
#include "cpp-terminal/platforms/file.hpp"
#include "cpp-terminal/platforms/input.hpp"
#include "cpp-terminal/platforms/sigwinch.hpp"

#include <mutex>
#include <string>

#if defined(_WIN32)
Term::Button::Action getAction(const std::int32_t& old_state, const std::int32_t& state, const std::int32_t& type)
{
  if(((state - old_state) >> (type - 1)) == 1) return Term::Button::Action::Pressed;
  else if(((old_state - state) >> (type - 1)) == 1)
    return Term::Button::Action::Released;
  else
    return Term::Button::Action::None;
}
Term::Button setButton(const std::int32_t& old_state, const std::int32_t& state)
{
  Term::Button::Action action;
  action = getAction(old_state, state, FROM_LEFT_1ST_BUTTON_PRESSED);
  if(action != Term::Button::Action::None) return Term::Button(Term::Button::Type::Left, action);

  action = getAction(old_state, state, FROM_LEFT_2ND_BUTTON_PRESSED);
  if(action != Term::Button::Action::None) return Term::Button(Term::Button::Type::Button1, action);

  action = getAction(old_state, state, FROM_LEFT_3RD_BUTTON_PRESSED);
  if(action != Term::Button::Action::None) return Term::Button(Term::Button::Type::Button2, action);

  action = getAction(old_state, state, FROM_LEFT_4TH_BUTTON_PRESSED);
  if(action != Term::Button::Action::None) return Term::Button(Term::Button::Type::Button3, action);

  action = getAction(old_state, state, RIGHTMOST_BUTTON_PRESSED);
  if(action != Term::Button::Action::None) return Term::Button(Term::Button::Type::Right, action);

  return Term::Button(Term::Button::Type::None, Term::Button::Action::None);
}

void sendString(Term::Private::BlockingQueue& events, std::wstring& str)
{
  if(!str.empty())
  {
    events.push(Term::Event(Term::Private::to_narrow(str.c_str())));
    str.clear();
  }
}

#endif

std::thread Term::Private::Input::m_thread = std::thread(Term::Private::Input::read_event);

Term::Private::BlockingQueue Term::Private::Input::m_events;

int Term::Private::Input::m_poll{-1};

void Term::Private::Input::init_thread()
{
  Term::Private::Sigwinch::unblockSigwinch();
#if defined(__linux__)
  m_poll = {::epoll_create1(EPOLL_CLOEXEC)};
  ::epoll_event signal;
  signal.events  = {EPOLLIN};
  signal.data.fd = {Term::Private::Sigwinch::get()};
  ::epoll_ctl(m_poll, EPOLL_CTL_ADD, Term::Private::Sigwinch::get(), &signal);
  ::epoll_event input;
  input.events  = {EPOLLIN};
  input.data.fd = {Term::Private::in.fd()};
  ::epoll_ctl(m_poll, EPOLL_CTL_ADD, Term::Private::in.fd(), &input);
#endif
}

void Term::Private::Input::read_event()
{
  init_thread();
  while(true)
  {
#if defined(_WIN32)
    WaitForSingleObject(Term::Private::in.handle(), INFINITE);
    read_raw();
#elif defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
    if(Term::Private::Sigwinch::isSigwinch()) m_events.push(screen_size());
    read_raw();
#else
    ::epoll_event ret;
    if(epoll_wait(m_poll, &ret, 1, -1) == 1)
    {
      if(Term::Private::Sigwinch::isSigwinch(ret.data.fd)) m_events.push(Term::Screen(screen_size()));
      else
        read_raw();
    }
#endif
  }
}

#if defined(_WIN32)
void Term::Private::Input::read_windows_key(const std::uint16_t& virtual_key_code, const std::uint32_t& control_key_state, const std::size_t& occurrence)
{
  // First check if we have ALT etc (CTRL is already done so skip it)
  Term::MetaKey toAdd{Term::MetaKey::Value::None};
  if(((control_key_state & LEFT_ALT_PRESSED) == LEFT_ALT_PRESSED) || ((control_key_state & RIGHT_ALT_PRESSED) == RIGHT_ALT_PRESSED)) toAdd += Term::MetaKey::Value::Alt;
  if(((control_key_state & LEFT_CTRL_PRESSED) == LEFT_CTRL_PRESSED) || ((control_key_state & RIGHT_CTRL_PRESSED) == RIGHT_CTRL_PRESSED)) toAdd += Term::MetaKey::Value::Ctrl;

  switch(virtual_key_code)
  {
    case VK_CANCEL:  //??
    case VK_CLEAR:   //??
    case VK_SHIFT:
    case VK_CONTROL:
    case VK_MENU:
    case VK_PAUSE:  //??
    case VK_CAPITAL:
    case VK_KANA:  //??
    //case VK_HANGUL: // Same
    case VK_JUNJA:  // ?
    case VK_FINAL:  // ?
    case VK_HANJA:
    //case VK_KANJI: // Same
    case VK_CONVERT:     // ?
    case VK_NONCONVERT:  // ?
    case VK_ACCEPT:      // ?
    case VK_MODECHANGE:  // ?
      break;
    case VK_PRIOR: m_events.push(std::move(toAdd + Term::Key(Key::Value::PageUp)), occurrence); break;
    case VK_NEXT: m_events.push(std::move(toAdd + Term::Key(Key::Value::PageDown)), occurrence); break;
    case VK_END: m_events.push(std::move(toAdd + Term::Key(Key::Value::End)), occurrence); break;
    case VK_HOME: m_events.push(std::move(toAdd + Term::Key(Key::Value::Home)), occurrence); break;
    case VK_LEFT: m_events.push(std::move(toAdd + Term::Key(Key::Value::ArrowLeft)), occurrence); break;
    case VK_UP: m_events.push(std::move(toAdd + Term::Key(Key::Value::ArrowUp)), occurrence); break;
    case VK_RIGHT: m_events.push(std::move(toAdd + Term::Key(Key::Value::ArrowRight)), occurrence); break;
    case VK_DOWN: m_events.push(std::move(toAdd + Term::Key(Key::Value::ArrowDown)), occurrence); break;
    case VK_SELECT:   //?
    case VK_PRINT:    //?
    case VK_EXECUTE:  //?
      break;
    case VK_SNAPSHOT: m_events.push(std::move(toAdd + Term::Key(Key::Value::PrintScreen)), occurrence); break;
    case VK_INSERT: m_events.push(std::move(toAdd + Term::Key(Key::Value::Insert)), occurrence); break;
    case VK_DELETE: m_events.push(std::move(toAdd + Term::Key(Key::Value::Del)), occurrence); break;
    case VK_HELP:   //?
    case VK_LWIN:   //Maybe allow to detect Windows key Left and right
    case VK_RWIN:   //Maybe allow to detect Windows key Left and right
    case VK_APPS:   //?
    case VK_SLEEP:  //?
      break;
    case VK_F1: m_events.push(std::move(toAdd + Term::Key(Key::Value::F1)), occurrence); break;
    case VK_F2: m_events.push(std::move(toAdd + Term::Key(Key::Value::F2)), occurrence); break;
    case VK_F3: m_events.push(std::move(toAdd + Term::Key(Key::Value::F3)), occurrence); break;
    case VK_F4: m_events.push(std::move(toAdd + Term::Key(Key::Value::F4)), occurrence); break;
    case VK_F5: m_events.push(std::move(toAdd + Term::Key(Key::Value::F5)), occurrence); break;
    case VK_F6: m_events.push(std::move(toAdd + Term::Key(Key::Value::F6)), occurrence); break;
    case VK_F7: m_events.push(std::move(toAdd + Term::Key(Key::Value::F7)), occurrence); break;
    case VK_F8: m_events.push(std::move(toAdd + Term::Key(Key::Value::F8)), occurrence); break;
    case VK_F9: m_events.push(std::move(toAdd + Term::Key(Key::Value::F9)), occurrence); break;
    case VK_F10: m_events.push(std::move(toAdd + Term::Key(Key::Value::F10)), occurrence); break;
    case VK_F11: m_events.push(std::move(toAdd + Term::Key(Key::Value::F11)), occurrence); break;
    case VK_F12: m_events.push(std::move(toAdd + Term::Key(Key::Value::F12)), occurrence); break;
    case VK_F13: m_events.push(std::move(toAdd + Term::Key(Key::Value::F13)), occurrence); break;
    case VK_F14: m_events.push(std::move(toAdd + Term::Key(Key::Value::F14)), occurrence); break;
    case VK_F15: m_events.push(std::move(toAdd + Term::Key(Key::Value::F15)), occurrence); break;
    case VK_F16: m_events.push(std::move(toAdd + Term::Key(Key::Value::F16)), occurrence); break;
    case VK_F17: m_events.push(std::move(toAdd + Term::Key(Key::Value::F17)), occurrence); break;
    case VK_F18: m_events.push(std::move(toAdd + Term::Key(Key::Value::F18)), occurrence); break;
    case VK_F19: m_events.push(std::move(toAdd + Term::Key(Key::Value::F19)), occurrence); break;
    case VK_F20: m_events.push(std::move(toAdd + Term::Key(Key::Value::F20)), occurrence); break;
    case VK_F21: m_events.push(std::move(toAdd + Term::Key(Key::Value::F21)), occurrence); break;
    case VK_F22: m_events.push(std::move(toAdd + Term::Key(Key::Value::F22)), occurrence); break;
    case VK_F23: m_events.push(std::move(toAdd + Term::Key(Key::Value::F23)), occurrence); break;
    case VK_F24: m_events.push(std::move(toAdd + Term::Key(Key::Value::F24)), occurrence); break;
    case VK_NUMLOCK:
    case VK_SCROLL:
    default: break;
  }
}
#endif

void Term::Private::Input::read_raw()
{
#ifdef _WIN32
  DWORD to_read{0};
  GetNumberOfConsoleInputEvents(Private::in.handle(), &to_read);
  if(to_read == 0) return;
  DWORD                     read{0};
  std::vector<INPUT_RECORD> events{to_read};
  if(!ReadConsoleInputW(Private::in.handle(), &events[0], to_read, &read) || read != to_read) Term::Exception("ReadFile() failed");
  std::wstring ret;
  bool         need_windows_size{false};
  for(std::size_t i = 0; i != read; ++i)
  {
    switch(events[i].EventType)
    {
      case KEY_EVENT:
      {
        if(events[i].Event.KeyEvent.bKeyDown)
        {
          if(events[i].Event.KeyEvent.uChar.UnicodeChar == 0) read_windows_key(events[i].Event.KeyEvent.wVirtualKeyCode, events[i].Event.KeyEvent.dwControlKeyState, events[i].Event.KeyEvent.wRepeatCount);
          else
            ret.append(events[i].Event.KeyEvent.wRepeatCount, events[i].Event.KeyEvent.uChar.UnicodeChar == Term::Key::Del ? static_cast<wchar_t>(Key(Term::Key::Value::Backspace)) : static_cast<wchar_t>(events[i].Event.KeyEvent.uChar.UnicodeChar));
        }
        break;
      }
      case FOCUS_EVENT:
      {
        sendString(m_events, ret);
        m_events.push(Event(Focus(static_cast<Term::Focus::Type>(events[i].Event.FocusEvent.bSetFocus))));
        break;
      }
      case MENU_EVENT:
      {
        sendString(m_events, ret);
        break;
      }
      case MOUSE_EVENT:
      {
        sendString(m_events, ret);
        static MOUSE_EVENT_RECORD old_state;
        if(events[i].Event.MouseEvent.dwEventFlags == MOUSE_WHEELED || events[i].Event.MouseEvent.dwEventFlags == MOUSE_HWHEELED)
          ;
        else if(old_state.dwButtonState == events[i].Event.MouseEvent.dwButtonState && old_state.dwMousePosition.X == events[i].Event.MouseEvent.dwMousePosition.X && old_state.dwMousePosition.Y == events[i].Event.MouseEvent.dwMousePosition.Y && old_state.dwEventFlags == events[i].Event.MouseEvent.dwEventFlags)
          break;
        std::int32_t state{static_cast<std::int32_t>(events[i].Event.MouseEvent.dwButtonState)};
        switch(events[i].Event.MouseEvent.dwEventFlags)
        {
          case 0:
          {
            m_events.push(Term::Mouse(setButton(static_cast<std::int32_t>(old_state.dwButtonState), state), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.Y), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.X)));
            ;
            break;
          }
          case MOUSE_MOVED:
          {
            m_events.push(Term::Mouse(setButton(static_cast<std::int32_t>(old_state.dwButtonState), state), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.Y), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.X)));
            ;
            break;
          }
          case DOUBLE_CLICK:
          {
            m_events.push(Term::Mouse(Term::Button(setButton(static_cast<std::int32_t>(old_state.dwButtonState), state).type(), Term::Button::Action::DoubleClicked), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.Y), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.X)));
            break;
          }
          case MOUSE_WHEELED:
          {
            if(state > 0) m_events.push(Term::Mouse(Button(Term::Button::Type::Wheel, Term::Button::Action::RolledUp), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.Y), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.X)));
            else
              m_events.push(Term::Mouse(Button(Term::Button::Type::Wheel, Term::Button::Action::RolledDown), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.Y), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.X)));
            break;
          }
          case MOUSE_HWHEELED:
          {
            if(state > 0) m_events.push(Term::Mouse(Button(Term::Button::Type::Wheel, Term::Button::Action::ToRight), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.Y), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.X)));
            else
              m_events.push(Term::Mouse(Button(Term::Button::Type::Wheel, Term::Button::Action::ToLeft), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.Y), static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.X)));
            break;
          }
          default: break;
        }
        old_state = events[i].Event.MouseEvent;
        break;
      }
      case WINDOW_BUFFER_SIZE_EVENT:
      {
        need_windows_size = true;  // if we send directly it's too much generations
        sendString(m_events, ret);
        break;
      }
      default: break;
    }
  }
  sendString(m_events, ret);
  if(need_windows_size == true) { m_events.push(screen_size()); }
#else
  Private::in.lockIO();
  std::string ret = Term::Private::in.read();
  Private::in.unlockIO();
  if(!ret.empty()) m_events.push(Event(ret.c_str()));
#endif
}

Term::Private::Input::Input() {}

void Term::Private::Input::startReading()
{
  static bool activated{false};
  if(!activated)
  {
    m_thread.detach();
    activated = true;
  }
}

Term::Event Term::Private::Input::getEvent() { return m_events.pop(); }

Term::Event Term::Private::Input::getEventBlocking()
{
  static std::mutex                   cv_m;
  static std::unique_lock<std::mutex> lk(cv_m);
  if(m_events.empty()) m_events.wait_for_events(lk);
  return m_events.pop();
}

static Term::Private::Input m_input;

Term::Event Term::read_event()
{
  m_input.startReading();
  return m_input.getEventBlocking();
}
