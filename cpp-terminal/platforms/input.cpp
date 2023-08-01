#if defined(_WIN32)
// clang-format off
  #include <windows.h>
  #include <stringapiset.h>
  // clang-format on
  #include <vector>
#elif defined(__APPLE__)
  #if !defined(_GLIBCXX_USE_NANOSLEEP)
    #define _GLIBCXX_USE_NANOSLEEP
  #endif
  #include <chrono>
  #include <thread>
  #include <cerrno>
  #include <csignal>
  #include <sys/ioctl.h>
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
#include "cpp-terminal/platforms/file.hpp"

#include <string>

#include <iostream>

#if !defined(_WIN32) && !defined(__APPLE__)
namespace Term
{

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

}  // namespace Term
#elif defined(__APPLE__)
namespace Term
{
 volatile std::sig_atomic_t m_signalStatus{0};
 static void sigwinchHandler(int sig)
 {
   if(sig==SIGWINCH) m_signalStatus=1;
 }
}
#endif

Term::Event Term::read_event()
{
#if defined(_WIN32)
  Term::Event ret;
  do
  {
    WaitForSingleObject(Term::Private::in.handle(),INFINITE);
    ret=std::move(Platform::read_raw());
  }
  while(ret.empty());
  return std::move(ret);
#elif defined(__APPLE__)
  static bool       enabled{false};
  if(!enabled)
  {
    ::sigset_t windows_event;
    sigemptyset(&windows_event);
    sigaddset(&windows_event, SIGWINCH);
    ::sigprocmask(SIG_BLOCK, &windows_event, nullptr);
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;
    sa.sa_handler =sigwinchHandler;
    sigaction(SIGWINCH,&sa,nullptr);
    ::sigprocmask(SIG_UNBLOCK, &windows_event, nullptr);
    enabled=true;
  }
  Term::Event ret;
  int wait{0};
  do
  {
    if(m_signalStatus==1)
    {
      m_signalStatus=0;
      return Term::Event(screen_size());
    }
    ret=Platform::read_raw();
    if(wait<=9)std::this_thread::sleep_for(std::chrono::milliseconds(++wait));
    else std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }while(ret.empty());
  return ret;


#else
  static bool       enabled{false};
  static Term::fd   epoll(::epoll_create1(EPOLL_CLOEXEC));
  static Term::fd   signal_fd(-1);
  static ::sigset_t windows_event;
  if(!enabled)
  {
    sigemptyset(&windows_event);
    sigaddset(&windows_event, SIGWINCH);
    ::sigprocmask(SIG_BLOCK, &windows_event, nullptr);
    signal_fd.set(::signalfd(-1, &windows_event, SFD_NONBLOCK | SFD_CLOEXEC));
    ::epoll_event signal;
    signal.data.fd = signal_fd.get();
    signal.events  = EPOLLIN;
    ::epoll_ctl(epoll.get(), EPOLL_CTL_ADD, signal_fd.get(), &signal);
    ::epoll_event input;
    input.events  = EPOLLIN;
    input.data.fd = Term::Private::in.fd();
    ::epoll_ctl(epoll.get(), EPOLL_CTL_ADD, Term::Private::in.fd(), &input);
    ::sigemptyset(&windows_event);
    enabled = true;
  }
  ::epoll_event ret;
  ::sigaddset(&windows_event, SIGWINCH);
  if(epoll_pwait(epoll.get(), &ret, 1, -1, &windows_event) == 1)
  {
    if(ret.data.fd == signal_fd.get())
    {
      ::signalfd_siginfo fdsi;
      read(signal_fd.get(), &fdsi, sizeof(fdsi));
      return Event(screen_size());
    }
    else
      return Platform::read_raw();
  }
  Term::Exception("Error on read_event.");
#endif
}

Term::Event Term::Platform::read_raw()
{
#ifdef _WIN32
  DWORD nread{0};
  GetNumberOfConsoleInputEvents(Private::in.handle(), &nread);
  if(nread >= 1)
  {
    std::string               ret;
    int                       processed{0};
    DWORD                     nre{0};
    std::vector<INPUT_RECORD> buf{nread};
    if(!ReadConsoleInputW(Private::in.handle(), &buf[0], buf.size(), &nre)) { Term::Exception("ReadFile() failed"); }
    for(std::size_t i = 0; i != nre; ++i)
    {
      switch(buf[i].EventType)
      {
        case KEY_EVENT:
        {
          //if(buf[i].Event.KeyEvent.wVirtualKeyCode != 0) break;  //skip them for now
          if(buf[i].Event.KeyEvent.bKeyDown)
          {
            std::size_t size_needed = WideCharToMultiByte(CP_UTF8, 0, &buf[i].Event.KeyEvent.uChar.UnicodeChar, -1, NULL, 0, NULL, NULL);
            std::string strTo(size_needed, '\0');
            WideCharToMultiByte(CP_UTF8, 0, &buf[i].Event.KeyEvent.uChar.UnicodeChar, 1, &strTo[0], size_needed, NULL, NULL);
            ret += strTo.c_str();
            ++processed;
            break;
            /*else
              switch(buf[i].Event.KeyEvent.wVirtualKeyCode)
              {
                case VK_CLEAR: return Event("");Key(Term::Key::Value::NUMERIC_5);
                case VK_PRIOR: return Key(Term::Key::Value::PAGE_UP);
                case VK_NEXT: return Key(Term::Key::Value::PAGE_DOWN);
                case VK_END: return Key(Term::Key::Value::END);
                case VK_HOME: return Key(Term::Key::Value::HOME);
                case VK_LEFT: return Key(Term::Key::Value::ARROW_LEFT);
                case VK_UP: return Key(Term::Key::Value::ARROW_UP);
                case VK_RIGHT: return Key(Term::Key::Value::ARROW_RIGHT);
                case VK_DOWN: return Key(Term::Key::Value::ARROW_DOWN);
                case VK_SNAPSHOT: return Key(Term::Key::Value::PRINT_SCREEN);
                case VK_INSERT: return Key(Term::Key::Value::INSERT);
                case VK_DELETE: return Key(Term::Key::Value::DEL);
                case VK_F1: return Key(Term::Key::Value::F1);
                case VK_F2: return Key(Term::Key::Value::F2);
                case VK_F3: return Key(Term::Key::Value::F3);
                case VK_F4: return Key(Term::Key::Value::F4);
                case VK_F5: return Key(Term::Key::Value::F5);
                case VK_F6: return Key(Term::Key::Value::F6);
                case VK_F7: return Key(Term::Key::Value::F7);
                case VK_F8: return Key(Term::Key::Value::F8);
                case VK_F9: return Key(Term::Key::Value::F9);
                case VK_F10: return Key(Term::Key::Value::F10);
                case VK_F11: return Key(Term::Key::Value::F11);
                case VK_F12: return Key(Term::Key::Value::F12);
                case VK_F13: return Key(Term::Key::Value::F13);
                case VK_F14: return Key(Term::Key::Value::F14);
                case VK_F15: return Key(Term::Key::Value::F15);
                case VK_F16: return Key(Term::Key::Value::F16);
                case VK_F17: return Key(Term::Key::Value::F17);
                case VK_F18: return Key(Term::Key::Value::F18);
                case VK_F19: return Key(Term::Key::Value::F19);
                case VK_F20: return Key(Term::Key::Value::F20);
                case VK_F21: return Key(Term::Key::Value::F21);
                case VK_F22: return Key(Term::Key::Value::F22);
                case VK_F23: return Key(Term::Key::Value::F23);
                case VK_F24: return Key(Term::Key::Value::F24);
              }*/
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
    else
      return Event();
  }
  else
    return Event();
#else
  std::size_t nread{0};
  ::ioctl(Private::in.fd(), FIONREAD, &nread);
  std::string ret(nread, '\0');
  errno = 0;
  ::ssize_t nrea{::read(Private::in.fd(), &ret[0], ret.size())};
  if(nrea == -1 && errno != EAGAIN) { throw Term::Exception("read() failed"); }
  return Event(ret.c_str());
#endif
}
