#pragma once

#include "cpp-terminal/stream.hpp"

namespace Term
{

extern TIstream& cin;
extern TOstream& cout;
extern TOstream& cerr;
extern TOstream& clog;

class StreamInitializer
{
public:
  StreamInitializer();
  static void init();
  ~StreamInitializer();
  StreamInitializer(const StreamInitializer&)            = delete;
  StreamInitializer& operator=(const StreamInitializer&) = delete;
  StreamInitializer(StreamInitializer&&)                 = delete;
  StreamInitializer& operator=(StreamInitializer&&)      = delete;

private:
  static int m_counter;
};

static StreamInitializer m_streams;

}  // namespace Term
