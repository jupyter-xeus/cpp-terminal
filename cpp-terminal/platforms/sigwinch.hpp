#pragma once

namespace Term
{
namespace Private
{

class Sigwinch
{
public:
  static void registerSigwinch();
  static void blockSigwinch();
  static void unblockSigwinch();
  static bool isSigwinch(const int& fd = -1);
  static int  get();

private:
  static int m_fd;
};

}  // namespace Private
}  // namespace Term
