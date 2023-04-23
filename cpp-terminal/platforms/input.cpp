#ifdef _WIN32
  #include <vector>
  #include <windows.h>
#else
  #include <cerrno>
  #include <csignal>
  #include <unistd.h>
#endif

#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"

#include <string>

#if !defined(_WIN32)
namespace
{
volatile std::sig_atomic_t gSignalStatus;
}

static void sigwinchHandler(int sig)
{
  if(sig == SIGWINCH) gSignalStatus = 1;
}
#endif

char Term::Platform::read_raw_stdin()
{
  char c = getchar();
  if(c >= 0) { return c; }
  else if(c == EOF)
  {
    // In non-raw (blocking) mode this happens when the input file
    // ends. In such a case, return the End of Transmission (EOT)
    // character (Ctrl-D)
    return 0x04;
  }
  else { throw Term::Exception("getchar() failed"); }
}

Term::Event Term::Platform::read_raw()
{
#ifdef _WIN32
  DWORD nread{0};
  GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE), &nread);
  if(nread >= 1)
  {
    DWORD                     nre{0};
    std::vector<INPUT_RECORD> buf{nread};
    if(!ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &buf[0], buf.size(), &nre)) { Term::Exception("ReadFile() failed"); }
    std::string ret(nre, '\0');
    int         processed{0};
    for(std::size_t i = 0; i != nre; ++i)
    {
      switch(buf[i].EventType)
      {
        case KEY_EVENT:
        {
          WORD skip = buf[i].Event.KeyEvent.wVirtualKeyCode;  //skip them for now
          if(skip == VK_SHIFT || skip == VK_LWIN || skip == VK_RWIN || skip == VK_APPS || skip == VK_CONTROL || skip == VK_MENU || skip == VK_CAPITAL) break;
          if(buf[i].Event.KeyEvent.bKeyDown)
          {
            if(buf[i].Event.KeyEvent.uChar.AsciiChar != 0) ret[processed] = buf[i].Event.KeyEvent.uChar.AsciiChar;
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
        case MENU_EVENT:
        case MOUSE_EVENT:
        case WINDOW_BUFFER_SIZE_EVENT:
        {
          COORD coord{buf[i].Event.WindowBufferSizeEvent.dwSize};
          return Event(Screen(coord.Y, coord.X));
        }
        default: continue;
      }
    }
    return Event(ret.c_str());
  }
  else
    return Event();
#else
  static bool activated{false};
  if(!activated)
  {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags   = 0;
    sa.sa_handler = sigwinchHandler;
    if(sigaction(SIGWINCH, &sa, NULL) == -1) throw Term::Exception("signal() failed");
    else
      activated = true;
  }

  if(gSignalStatus == 1)
  {
    gSignalStatus = 0;
    return Event(screen_size());
  }
  else
  {
    std::string ret(4096, '\0');  // Max for cin
    errno = 0;
    ::ssize_t nread{::read(0, &ret[0], ret.size())};
    if(nread == -1 && errno != EAGAIN) { throw Term::Exception("read() failed"); }
    if(nread >= 1) return Event(ret.c_str());
    else
      return Event();
  }
#endif
}
