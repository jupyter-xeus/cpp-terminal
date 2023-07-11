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
  void init();
  ~StreamInitializer();

private:
  static int m_counter;
};

static StreamInitializer m_streams;

}  // namespace Term
