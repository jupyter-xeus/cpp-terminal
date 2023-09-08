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
    Term::Event ret;
    WaitForSingleObject(Term::Private::in.handle(), INFINITE);
    ret = read_raw();
    if(!ret.empty()) m_events.push(std::move(ret));
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

Term::Event Term::Private::Input::read_raw()
{
#ifdef _WIN32
  DWORD to_read{0};
  GetNumberOfConsoleInputEvents(Private::in.handle(), &to_read);
  if(to_read == 0) return Term::Event();
  // If it's one event it's easy
  else if(to_read == 1)
  {
    DWORD        read{0};
    INPUT_RECORD event{};
    if(!ReadConsoleInputW(Private::in.handle(), &event, to_read, &read) || read != to_read) Term::Exception("ReadFile() failed");
    switch(event.EventType)
    {
      case KEY_EVENT:
      {
        if(event.Event.KeyEvent.bKeyDown)
        {
          // First check if we have ALT etc (CTRL is already done so skip it)
          Term::MetaKey toAdd{Term::MetaKey::Value::None};
          if(((event.Event.KeyEvent.dwControlKeyState & LEFT_ALT_PRESSED) == LEFT_ALT_PRESSED) || ((event.Event.KeyEvent.dwControlKeyState & RIGHT_ALT_PRESSED) == RIGHT_ALT_PRESSED)) toAdd = std::move(Term::MetaKey::Alt);

          if(event.Event.KeyEvent.uChar.UnicodeChar == 0)
          {
            switch(event.Event.KeyEvent.wVirtualKeyCode)
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
                return Term::Event();
              case VK_PRIOR: return std::move(Term::Key(Key::Value::PageUp));
              case VK_NEXT: return std::move(Term::Key(Key::Value::PageDown));
              case VK_END: return std::move(Term::Key(Key::Value::End));
              case VK_HOME: return std::move(Term::Key(Key::Value::Home));
              case VK_LEFT: return std::move(Term::Key(Key::Value::ArrowLeft));
              case VK_UP: return std::move(Term::Key(Key::Value::ArrowUp));
              case VK_RIGHT: return std::move(Term::Key(Key::Value::ArrowRight));
              case VK_DOWN: return std::move(Term::Key(Key::Value::ArrowDown));
              case VK_SELECT:   //?
              case VK_PRINT:    //?
              case VK_EXECUTE:  //?
                return Term::Event();
              case VK_SNAPSHOT: return std::move(Term::Key(Key::Value::PrintScreen));
              case VK_INSERT: return std::move(Term::Key(Key::Value::Insert));
              case VK_DELETE: return std::move(Term::Key(Key::Value::Del));
              case VK_HELP:   //?
              case VK_LWIN:   //Maybe allow to detect Windows key Left and right
              case VK_RWIN:   //Maybe allow to detect Windows key Left and right
              case VK_APPS:   //?
              case VK_SLEEP:  //?
                return Term::Event();
              case VK_F1: return std::move(Term::Key(Key::Value::F1));
              case VK_F2: return std::move(Term::Key(Key::Value::F2));
              case VK_F3: return std::move(Term::Key(Key::Value::F3));
              case VK_F4: return std::move(Term::Key(Key::Value::F4));
              case VK_F5: return std::move(Term::Key(Key::Value::F5));
              case VK_F6: return std::move(Term::Key(Key::Value::F6));
              case VK_F7: return std::move(Term::Key(Key::Value::F7));
              case VK_F8: return std::move(Term::Key(Key::Value::F8));
              case VK_F9: return std::move(Term::Key(Key::Value::F9));
              case VK_F10: return std::move(Term::Key(Key::Value::F10));
              case VK_F11: return std::move(Term::Key(Key::Value::F11));
              case VK_F12: return std::move(Term::Key(Key::Value::F12));
              case VK_F13: return std::move(Term::Key(Key::Value::F13));
              case VK_F14: return std::move(Term::Key(Key::Value::F14));
              case VK_F15: return std::move(Term::Key(Key::Value::F15));
              case VK_F16: return std::move(Term::Key(Key::Value::F16));
              case VK_F17: return std::move(Term::Key(Key::Value::F17));
              case VK_F18: return std::move(Term::Key(Key::Value::F18));
              case VK_F19: return std::move(Term::Key(Key::Value::F19));
              case VK_F20: return std::move(Term::Key(Key::Value::F20));
              case VK_F21: return std::move(Term::Key(Key::Value::F21));
              case VK_F22: return std::move(Term::Key(Key::Value::F22));
              case VK_F23: return std::move(Term::Key(Key::Value::F23));
              case VK_F24: return std::move(Term::Key(Key::Value::F24));
              case VK_NUMLOCK:
              case VK_SCROLL:
              default: return Term::Event();
            }
          }
          else
          {
            std::string ret{to_utf8(&event.Event.KeyEvent.uChar.UnicodeChar)};
            if(ret.size() == 1)
            {
              //Special case DEL should be Backspace
              if(ret[0] == Term::Key::Del) return toAdd + Key(Term::Key::Value::Backspace);
              else
                return toAdd + Key(static_cast<Term::Key::Value>(ret[0]));
            }
            else
              return Term::Event(ret);
          }
        }
        else
          return Term::Event();
      }
      case FOCUS_EVENT:
      {
        return Term::Event();
      }
      case MENU_EVENT:
      {
        return Term::Event();
      }
      case MOUSE_EVENT:
      {
        return Term::Event();
      }
      case WINDOW_BUFFER_SIZE_EVENT:
      {
        return Event(screen_size());
      }
    }
  }
  else  // Here we have big problems we need to change this function to return std::vector<Event>;
  {
    DWORD                     read{0};
    std::vector<INPUT_RECORD> events{to_read};
    if(!ReadConsoleInputW(Private::in.handle(), &events[0], to_read, &read) || read != to_read) Term::Exception("ReadFile() failed");
    std::string ret;
    int         processed{0};
    for(std::size_t i = 0; i != read; ++i)
    {
      switch(events[i].EventType)
      {
        case KEY_EVENT:
        {
          if(events[i].Event.KeyEvent.bKeyDown)
          {
            if(events[i].Event.KeyEvent.uChar.UnicodeChar == 0) break;
            else
            {
              std::string ch{to_utf8(&events[i].Event.KeyEvent.uChar.UnicodeChar)};
              if(ch.size() == 1 && ch[0] == Term::Key::Del) return Key(Term::Key::Value::Backspace);
              else
                ret += ch;
              ++processed;
            }
          }
          else
            break;
        }
        case FOCUS_EVENT:
        {
          break;
        }
        case MENU_EVENT:
        {
          break;
        }
        case MOUSE_EVENT:
        {
          break;
        }
        case WINDOW_BUFFER_SIZE_EVENT:
        {
          return Event(screen_size());
        }
      }
    }
    if(processed >= 1) return Event(ret);
  }
  return Event();
#else
  std::size_t nread{0};
  ::ioctl(Private::in.fd(), FIONREAD, &nread);
  if(nread == 0) return Term::Event();  //FIXME
  std::string ret(nread, '\0');
  errno = 0;
  ::ssize_t nrea{::read(Private::in.fd(), &ret[0], nread)};
  if(nrea == -1 && errno != EAGAIN) { throw Term::Exception("read() failed"); }
  m_events.push(Event(ret.c_str()));
  return Event(ret.c_str());  //FIXME
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
