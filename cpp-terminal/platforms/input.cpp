#if defined(_WIN32)
  // clang-format off
  #include <windows.h>
  #include <stringapiset.h>
  // clang-format on
  #include "cpp-terminal/platforms/conversion.hpp"

  #include <vector>
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
  Term::Button::Action getAction(const std::int32_t& old_state,const std::int32_t& state, const std::int32_t& type)
  {
  if(((state-old_state)>>(type-1))==1)  return Term::Button::Action::Pressed;
  else if(((old_state-state)>>(type-1))==1) return Term::Button::Action::Released;
  else return Term::Button::Action::None;
  }
  void setButton(std::array<Term::Button,11>& buttons,const std::int32_t& old_state,const std::int32_t& state, const std::int32_t& type)
  {
  Term::Button::Action action;
  action= getAction(old_state,state,FROM_LEFT_1ST_BUTTON_PRESSED);
  if(action==Term::Button::Action::Pressed &&  type == DOUBLE_CLICK) action= Term::Button::Action::DoubleClicked;
  buttons[static_cast<std::size_t>(Term::Button::Type::Left)] = Term::Button(Term::Button::Type::Left,action);

  action= getAction(old_state,state,FROM_LEFT_2ND_BUTTON_PRESSED);
  if(action==Term::Button::Action::Pressed &&  type == DOUBLE_CLICK) action= Term::Button::Action::DoubleClicked;
  buttons[static_cast<std::size_t>(Term::Button::Type::Button1)] = Term::Button(Term::Button::Type::Button1,action);

  action= getAction(old_state,state,FROM_LEFT_3RD_BUTTON_PRESSED);
  if(action==Term::Button::Action::Pressed &&  type == DOUBLE_CLICK) action= Term::Button::Action::DoubleClicked;
  buttons[static_cast<std::size_t>(Term::Button::Type::Button2)] = Term::Button(Term::Button::Type::Button2,action);

  action= getAction(old_state,state,FROM_LEFT_4TH_BUTTON_PRESSED);
  if(action==Term::Button::Action::Pressed &&  type == DOUBLE_CLICK) action= Term::Button::Action::DoubleClicked;
  buttons[static_cast<std::size_t>(Term::Button::Type::Button3)] = Term::Button(Term::Button::Type::Button3,action);

  action= getAction(old_state,state,RIGHTMOST_BUTTON_PRESSED);
  if(action==Term::Button::Action::Pressed &&  type == DOUBLE_CLICK) action= Term::Button::Action::DoubleClicked;
  buttons[static_cast<std::size_t>(Term::Button::Type::Right)] = Term::Button(Term::Button::Type::Right,action);

  buttons[static_cast<std::size_t>(Term::Button::Type::Wheel)] = Term::Button(Term::Button::Type::Wheel,Term::Button::Action::None);
  buttons[static_cast<std::size_t>(Term::Button::Type::Button4)] = Term::Button(Term::Button::Type::Button4,Term::Button::Action::None);
  buttons[static_cast<std::size_t>(Term::Button::Type::Button5)] = Term::Button(Term::Button::Type::Button5,Term::Button::Action::None);
  buttons[static_cast<std::size_t>(Term::Button::Type::Button6)] = Term::Button(Term::Button::Type::Button6,Term::Button::Action::None);
  buttons[static_cast<std::size_t>(Term::Button::Type::Button7)] = Term::Button(Term::Button::Type::Button7,Term::Button::Action::None);
  buttons[static_cast<std::size_t>(Term::Button::Type::Button8)] = Term::Button(Term::Button::Type::Button8,Term::Button::Action::None);
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
  std::string ret;
  bool        need_windows_size{false};
  for(std::size_t i = 0; i != read; ++i)
  {
    switch(events[i].EventType)
    {
      case KEY_EVENT:
      {
        if(events[i].Event.KeyEvent.bKeyDown)
        {
          if(events[i].Event.KeyEvent.uChar.UnicodeChar == 0) { read_windows_key(events[i].Event.KeyEvent.wVirtualKeyCode, events[i].Event.KeyEvent.dwControlKeyState, events[i].Event.KeyEvent.wRepeatCount); }
          else
          {
            if(events[i].Event.KeyEvent.uChar.UnicodeChar <= 127)  //MAYBE BUG in to_utf8 (me or Windaube)
            {
              if(events[i].Event.KeyEvent.uChar.UnicodeChar == Term::Key::Del) ret.append(events[i].Event.KeyEvent.wRepeatCount, static_cast<char>(Key(Term::Key::Value::Backspace)));
              else
                ret.append(events[i].Event.KeyEvent.wRepeatCount, static_cast<char>(events[i].Event.KeyEvent.uChar.UnicodeChar));
            }
            else
              for(std::size_t j = 0; j != events[i].Event.KeyEvent.wRepeatCount; ++j) ret.append(to_utf8(&events[i].Event.KeyEvent.uChar.UnicodeChar));
          }
          break;
        }
        else
          break;
      }
      case FOCUS_EVENT:
      {
        if(!ret.empty())
        {
          m_events.push(Term::Event(ret));
          ret.clear();
        }
        m_events.push(Event(Focus(static_cast<Term::Focus::Type>(events[i].Event.FocusEvent.bSetFocus))));
        break;
      }
      case MENU_EVENT:
      {
        if(!ret.empty())
        {
          m_events.push(Term::Event(ret));
          ret.clear();
        }
        break;
      }
      case MOUSE_EVENT:
      {
        if(!ret.empty())
        {
          m_events.push(Term::Event(ret));
          ret.clear();
        }
        static std::int32_t old_state{0};
        std::int32_t state{static_cast<std::int32_t>(events[i].Event.MouseEvent.dwButtonState)};
        std::array<Term::Button, 11> buttons;
        switch(events[i].Event.MouseEvent.dwEventFlags)
        {
          case 0:
          {
            setButton(buttons,old_state,state,0);
            break;
          }
          case MOUSE_MOVED:
          {
            setButton(buttons,old_state,state,MOUSE_MOVED);
            break;
          }
          case DOUBLE_CLICK:
          {
            setButton(buttons,old_state,state,DOUBLE_CLICK);
            break;
          }
          case MOUSE_WHEELED:
          {
            setButton(buttons,old_state,state,MOUSE_WHEELED);
            if(state>0) buttons[static_cast<std::size_t>(Term::Button::Type::Wheel)]=Button(Term::Button::Type::Wheel,Term::Button::Action::RolledUp);
            else buttons[static_cast<std::size_t>(Term::Button::Type::Wheel)]=Button(Term::Button::Type::Wheel,Term::Button::Action::RolledDown);break;
          }
          case MOUSE_HWHEELED:
          {
            setButton(buttons,old_state,state,MOUSE_HWHEELED);
            if(state>0) buttons[static_cast<std::size_t>(Term::Button::Type::Wheel)]=Button(Term::Button::Type::Wheel,Term::Button::Action::ToRight);
            else buttons[static_cast<std::size_t>(Term::Button::Type::Wheel)]=Button(Term::Button::Type::Wheel,Term::Button::Action::ToLeft);
            break;
          }
          default : break;
        }
        m_events.push(Term::Mouse(buttons,static_cast<std::uint16_t >(events[i].Event.MouseEvent.dwMousePosition.Y),static_cast<std::uint16_t>(events[i].Event.MouseEvent.dwMousePosition.X)));
        old_state=state;
        break;
      }
      case WINDOW_BUFFER_SIZE_EVENT:
      {
        need_windows_size = true;  // if we send directly it's too much generations
        if(!ret.empty())
        {
          m_events.push(Term::Event(ret));
          ret.clear();
        }
        break;
      }
      default: break;
    }
  }
  if(!ret.empty()) { m_events.push(Term::Event(ret.c_str())); }
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


