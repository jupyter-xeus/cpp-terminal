#if defined(_WIN32)
// clang-format off
  #include <windows.h>
  #include <stringapiset.h>
  // clang-format on
  #include <vector>
#elif defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
  #if !defined(_GLIBCXX_USE_NANOSLEEP)
    #define _GLIBCXX_USE_NANOSLEEP
  #endif
  #include <cerrno>
  #include <chrono>
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
#include "cpp-terminal/platforms/file.hpp"
#include "cpp-terminal/platforms/input.hpp"

#include <string>
#include <thread>

namespace Term
{
namespace Private
{
template<typename T> class BlockingQueue
{
private:
  std::mutex    mutex_;
  std::queue<T> queue_;

public:
  T pop()
  {
    this->mutex_.lock();
    T value = this->queue_.front();
    this->queue_.pop();
    this->mutex_.unlock();
    return value;
  }

  void push(const T& value)
  {
    this->mutex_.lock();
    this->queue_.push(value);
    this->mutex_.unlock();
  }

  void push(T&& value)
  {
    this->mutex_.lock();
    this->queue_.push(std::move(value));
    this->mutex_.unlock();
  }

  bool empty()
  {
    this->mutex_.lock();
    bool check = this->queue_.empty();
    this->mutex_.unlock();
    return check;
  }
  std::size_t size()
  {
    this->mutex_.lock();
    std::size_t check = this->queue_.size();
    this->mutex_.unlock();
    return check;
  }
};
}  // namespace Private
}  // namespace Term

std::thread Term::Private::Input::m_thread = std::thread(Term::Private::Input::read_event);

Term::Private::BlockingQueue<Term::Event> Term::Private::Input::m_events;

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
#elif defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
namespace Term
{
volatile std::sig_atomic_t m_signalStatus{0};
static void                sigwinchHandler(int sig)
{
  if(sig == SIGWINCH) m_signalStatus = 1;
}
}  // namespace Term
#endif

void Term::Private::Input::read_event()
{
  while(true)
  {
#if defined(_WIN32)
    Term::Event ret;
    WaitForSingleObject(Term::Private::in.handle(), INFINITE);
    ret = std::move(read_raw());
    if(!ret.empty()) m_events.push(std::move(ret));
#elif defined(__APPLE__) || defined(__wasm__) || defined(__wasm) || defined(__EMSCRIPTEN__)
    static bool enabled{false};
    if(!enabled)
    {
      ::sigset_t windows_event;
      sigemptyset(&windows_event);
      sigaddset(&windows_event, SIGWINCH);
      ::sigprocmask(SIG_BLOCK, &windows_event, nullptr);
      struct sigaction sa;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags   = 0;
      sa.sa_handler = sigwinchHandler;
      sigaction(SIGWINCH, &sa, nullptr);
      ::sigprocmask(SIG_UNBLOCK, &windows_event, nullptr);
      enabled = true;
    }
    Term::Event ret;
    int         wait{0};
    do {
      if(m_signalStatus == 1)
      {
        m_signalStatus = 0;
        return Term::Event(screen_size());
      }
      ret = Platform::read_raw();
      if(wait <= 9) std::this_thread::sleep_for(std::chrono::milliseconds(++wait));
      else
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } while(ret.empty());
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
        m_events.push(screen_size());
      }
      else
        m_events.push(read_raw());
    }
#endif
  }
}

Term::Event Term::Private::Input::read_raw()
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
  std::string ret(nread, '\0');
  errno = 0;
  ::ssize_t nrea{::read(Private::in.fd(), &ret[0], ret.size())};
  if(nrea == -1 && errno != EAGAIN) { throw Term::Exception("read() failed"); }
  return Event(ret.c_str());
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

Term::Event Term::Private::Input::getEvent()
{
  if(m_events.empty()) return Term::Event();
  else
    return m_events.pop();
}

Term::Event Term::Private::Input::getEventBlocking()
{
  while(m_events.empty()) continue;
  return m_events.pop();
}

Term::Event Term::read_event()
{
  static Term::Private::Input m_input;
  m_input.startReading();
  return m_input.getEventBlocking();
}
