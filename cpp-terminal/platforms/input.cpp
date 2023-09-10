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
  #include <cerrno>
  #include <csignal>
  #include <memory>
  #include <sys/epoll.h>
  #include <sys/ioctl.h>
  #include <sys/signalfd.h>
  #include <unistd.h>
#endif

#include "cpp-terminal/event.hpp"
#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/platforms/blocking_queue.hpp"
#include "cpp-terminal/platforms/file.hpp"
#include "cpp-terminal/platforms/input.hpp"

#include <mutex>
#include <iostream>
#include <string>

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
#elif defined(_WIN32)
#else
class fd
{
public:
  fd(const int& i) : m_fd(i) {}
  ~fd() { ::close(m_fd); }
  void set(const int& i) { m_fd = i; }
  int  get() { return m_fd; }

private:
  int m_fd{-1};
};
#endif

}  // namespace Private
}  // namespace Term

std::thread Term::Private::Input::m_thread = std::thread(Term::Private::Input::read_event);

Term::Private::BlockingQueue Term::Private::Input::m_events;

void Term::Private::Input::read_event()
{
  while(true)
  {
#if defined(_WIN32)
    WaitForSingleObject(Term::Private::in.handle(), INFINITE);
    read_raw();
#elif defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
    ::sigset_t windows_event;
    sigemptyset(&windows_event);
    sigaddset(&windows_event, SIGWINCH);
    ::sigprocmask(SIG_UNBLOCK, &windows_event, nullptr);
    if(Term::Private::m_signalStatus == 1)
    {
      Term::Private::m_signalStatus = 0;
      m_events.push(screen_size());
    }
    read_raw();
#else
    static bool              enabled{false};
    static Term::Private::fd epoll(::epoll_create1(EPOLL_CLOEXEC));
    static Term::Private::fd signal_fd(-1);
    static ::sigset_t        windows_event;
    if(!enabled)
    {
      sigemptyset(&windows_event);
      sigaddset(&windows_event, SIGWINCH);
      ::sigprocmask(SIG_BLOCK, &windows_event, nullptr);
      signal_fd.set(::signalfd(-1, &windows_event, SFD_NONBLOCK | SFD_CLOEXEC));
      ::epoll_event signal;
      signal.events  = EPOLLIN;
      signal.data.fd = signal_fd.get();
      ::epoll_ctl(epoll.get(), EPOLL_CTL_ADD, signal_fd.get(), &signal);
      ::epoll_event input;
      input.events  = EPOLLIN;
      input.data.fd = Term::Private::in.fd();
      ::epoll_ctl(epoll.get(), EPOLL_CTL_ADD, Term::Private::in.fd(), &input);
      ::sigprocmask(SIG_UNBLOCK, &windows_event, nullptr);
      enabled = true;
    }
    ::epoll_event ret;
    ::sigemptyset(&windows_event);
    ::sigaddset(&windows_event, SIGWINCH);
    if(epoll_wait(epoll.get(), &ret, 1, -1) == 1)
    {
      if(ret.data.fd == signal_fd.get())
      {
        ::signalfd_siginfo fdsi;
        read(signal_fd.get(), &fdsi, sizeof(fdsi));
        m_events.push(Term::Screen(screen_size()));
      }
      else
        read_raw();
    }
#endif
  }
}


 void Term::Private::Input::read_windows_key(const std::uint16_t& virtual_key_code, const std::uint32_t& control_key_state, const std::size_t& occurence)
{
#ifdef _WIN32
  // First check if we have ALT etc (CTRL is already done so skip it)
  Term::MetaKey toAdd{Term::MetaKey::Value::None};
  if(((control_key_state & LEFT_ALT_PRESSED) == LEFT_ALT_PRESSED) || ((control_key_state & RIGHT_ALT_PRESSED) == RIGHT_ALT_PRESSED)) toAdd += std::move(Term::MetaKey::Alt);
  if(((control_key_state & LEFT_CTRL_PRESSED) == LEFT_CTRL_PRESSED) || ((control_key_state & RIGHT_CTRL_PRESSED) == RIGHT_CTRL_PRESSED)) toAdd += std::move(Term::MetaKey::Ctrl);

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
    case VK_PRIOR: m_events.push(std::move(toAdd + Term::Key(Key::Value::PageUp)), occurence); break;
    case VK_NEXT: m_events.push(std::move(toAdd + Term::Key(Key::Value::PageDown)), occurence); break;
    case VK_END: m_events.push(std::move(toAdd + Term::Key(Key::Value::End)), occurence); break;
    case VK_HOME: m_events.push(std::move(toAdd + Term::Key(Key::Value::Home)), occurence); break;
    case VK_LEFT: m_events.push(std::move(toAdd + Term::Key(Key::Value::ArrowLeft)), occurence); break;
    case VK_UP: m_events.push(std::move(toAdd + Term::Key(Key::Value::ArrowUp)), occurence); break;
    case VK_RIGHT: m_events.push(std::move(toAdd + Term::Key(Key::Value::ArrowRight)), occurence); break;
    case VK_DOWN: m_events.push(std::move(toAdd + Term::Key(Key::Value::ArrowDown)), occurence); break;
    case VK_SELECT:   //?
    case VK_PRINT:    //?
    case VK_EXECUTE:  //?
       break;
    case VK_SNAPSHOT: m_events.push(std::move(toAdd + Term::Key(Key::Value::PrintScreen)), occurence); break;
    case VK_INSERT: m_events.push(std::move(toAdd + Term::Key(Key::Value::Insert)), occurence); break;
    case VK_DELETE: m_events.push(std::move(toAdd + Term::Key(Key::Value::Del)), occurence); break;
    case VK_HELP:   //?
    case VK_LWIN:   //Maybe allow to detect Windows key Left and right
    case VK_RWIN:   //Maybe allow to detect Windows key Left and right
    case VK_APPS:   //?
    case VK_SLEEP:  //?
       break;
    case VK_F1: m_events.push(std::move(toAdd + Term::Key(Key::Value::F1)), occurence); break;
    case VK_F2: m_events.push(std::move(toAdd + Term::Key(Key::Value::F2)), occurence); break;
    case VK_F3: m_events.push(std::move(toAdd + Term::Key(Key::Value::F3)), occurence); break;
    case VK_F4: m_events.push(std::move(toAdd + Term::Key(Key::Value::F4)), occurence); break;
    case VK_F5: m_events.push(std::move(toAdd + Term::Key(Key::Value::F5)), occurence); break;
    case VK_F6: m_events.push(std::move(toAdd + Term::Key(Key::Value::F6)), occurence); break;
    case VK_F7: m_events.push(std::move(toAdd + Term::Key(Key::Value::F7)), occurence); break;
    case VK_F8: m_events.push(std::move(toAdd + Term::Key(Key::Value::F8)), occurence); break;
    case VK_F9: m_events.push(std::move(toAdd + Term::Key(Key::Value::F9)), occurence); break;
    case VK_F10: m_events.push(std::move(toAdd + Term::Key(Key::Value::F10)), occurence); break;
    case VK_F11: m_events.push(std::move(toAdd + Term::Key(Key::Value::F11)), occurence); break;
    case VK_F12: m_events.push(std::move(toAdd + Term::Key(Key::Value::F12)), occurence); break;
    case VK_F13: m_events.push(std::move(toAdd + Term::Key(Key::Value::F13)), occurence); break;
    case VK_F14: m_events.push(std::move(toAdd + Term::Key(Key::Value::F14)), occurence); break;
    case VK_F15: m_events.push(std::move(toAdd + Term::Key(Key::Value::F15)), occurence); break;
    case VK_F16: m_events.push(std::move(toAdd + Term::Key(Key::Value::F16)), occurence); break;
    case VK_F17: m_events.push(std::move(toAdd + Term::Key(Key::Value::F17)), occurence); break;
    case VK_F18: m_events.push(std::move(toAdd + Term::Key(Key::Value::F18)), occurence); break;
    case VK_F19: m_events.push(std::move(toAdd + Term::Key(Key::Value::F19)), occurence); break;
    case VK_F20: m_events.push(std::move(toAdd + Term::Key(Key::Value::F20)), occurence); break;
    case VK_F21: m_events.push(std::move(toAdd + Term::Key(Key::Value::F21)), occurence); break;
    case VK_F22: m_events.push(std::move(toAdd + Term::Key(Key::Value::F22)), occurence); break;
    case VK_F23: m_events.push(std::move(toAdd + Term::Key(Key::Value::F23)), occurence); break;
    case VK_F24: m_events.push(std::move(toAdd + Term::Key(Key::Value::F24)), occurence); break;
    case VK_NUMLOCK:
    case VK_SCROLL:
    default: break;
  }
#endif
}

void Term::Private::Input::read_raw()
{
#ifdef _WIN32
  DWORD to_read{0};
  GetNumberOfConsoleInputEvents(Private::in.handle(), &to_read);
  if(to_read == 0) return;
  DWORD read{0};
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
           if(events[i].Event.KeyEvent.uChar.UnicodeChar == 0 )
           {
             read_windows_key(events[i].Event.KeyEvent.wVirtualKeyCode, events[i].Event.KeyEvent.dwControlKeyState, events[i].Event.KeyEvent.wRepeatCount);
           }
           else
           {
             if(events[i].Event.KeyEvent.uChar.UnicodeChar <= 127)  //MAYBE BUG in to_utf8 (me or Windaube)
             {
               if(events[i].Event.KeyEvent.uChar.UnicodeChar == Term::Key::Del) ret.append(events[i].Event.KeyEvent.wRepeatCount, Key(Term::Key::Value::Backspace));
               ret.append(events[i].Event.KeyEvent.wRepeatCount, events[i].Event.KeyEvent.uChar.UnicodeChar);
             }
             else
               for(std::size_t j = 0; j != events[i].Event.KeyEvent.wRepeatCount;++j) ret.append(to_utf8(&events[i].Event.KeyEvent.uChar.UnicodeChar));
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
         break;
       }
       case WINDOW_BUFFER_SIZE_EVENT:
       {
         need_windows_size = true; // if we send directly it's too much generations
         if(!ret.empty())
         {
           m_events.push(Term::Event(ret));
           ret.clear();
         }
         break;
       }
       default:
         break;
    }
  }
  if(!ret.empty())
  {
    m_events.push(Term::Event(ret.c_str()));
  }
  if(need_windows_size == true) { m_events.push(screen_size()); }
#else
  std::size_t nread{0};
  ::ioctl(Private::in.fd(), FIONREAD, &nread);
  if(nread == 0) return;
  std::string ret(nread, '\0');
  errno = 0;
  ::ssize_t nrea{::read(Private::in.fd(), &ret[0], nread)};
  if(nrea == -1 && errno != EAGAIN) { throw Term::Exception("read() failed"); }
  m_events.push(Event(ret.c_str()));
#endif
}

Term::Private::Input::Input()
{
#if !defined(_WIN32)
  static ::sigset_t windows_event;
  sigemptyset(&windows_event);
  sigaddset(&windows_event, SIGWINCH);
  ::sigprocmask(SIG_BLOCK, &windows_event, nullptr);
#endif
#if defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
  static struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags   = 0;
  sa.sa_handler = Term::Private::sigwinchHandler;
  sigaction(SIGWINCH, &sa, nullptr);
#endif
}

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
  m_events.wait_for_events(lk);
  // fix for macos
  if(m_events.empty()) { m_events.wait_for_events(lk); }
  return m_events.pop();
}

Term::Event Term::read_event()
{
  static Term::Private::Input m_input;
  m_input.startReading();
  return m_input.getEventBlocking();
}
