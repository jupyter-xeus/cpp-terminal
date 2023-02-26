#ifdef _WIN32
  #include <windows.h>
#else
  #include <cerrno>
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

#include "cpp-terminal/exception.hpp"
#include "cpp-terminal/input.hpp"
#include "cpp-terminal/tty.hpp"

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

bool Term::Platform::read_raw(char* s)
{
  // do nothing when TTY is not connected
  if(!is_stdin_a_tty()) { return false; }
#ifdef _WIN32
  DWORD nread{0};
  GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE), &nread);
  if(nread >= 1)
  {
    INPUT_RECORD buf;
    if(!ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &buf, 1, &nread)) { Term::Exception("ReadFile() failed"); }
    if(nread == 1)
    {
      switch(buf.EventType)
      {
        case KEY_EVENT:
        {
          WORD skip = buf.Event.KeyEvent.wVirtualKeyCode;  //skip them for now
          if(skip == VK_SHIFT || skip == VK_LWIN || skip == VK_RWIN || skip == VK_APPS || skip == VK_CONTROL || skip == VK_MENU || skip == VK_CAPITAL) { return false; }
          if(buf.Event.KeyEvent.bKeyDown)
          {
            *s = buf.Event.KeyEvent.uChar.AsciiChar;
            return true;
          }
          else
            return false;
        }
        case FOCUS_EVENT:
        case MENU_EVENT:
        case MOUSE_EVENT:
        case WINDOW_BUFFER_SIZE_EVENT:
        default: return false;
      }
    }
    else { throw Term::Exception("kbhit() and ReadFile() inconsistent"); }
  }
  else
    return false;
#else
  ::ssize_t nread = ::read(0, s, 1);
  if(nread == -1 && errno != EAGAIN) { throw Term::Exception("read() failed"); }
  return (nread == 1);
#endif
}
