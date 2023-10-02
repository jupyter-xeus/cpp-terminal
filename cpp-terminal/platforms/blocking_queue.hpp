/*
* cpp-terminal
* C++ library for writing multiplatform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#pragma once

#include "cpp-terminal/event.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>

namespace Term
{

namespace Private
{

class BlockingQueue
{
private:
  std::mutex              mutex_;
  std::queue<Term::Event> queue_;
  std::condition_variable cv;

public:
  BlockingQueue()  = default;
  ~BlockingQueue() = default;
  Term::Event pop();
  void        push(const Term::Event& value, const std::size_t& occurrence = 1);
  void        push(const Term::Event&& value, const std::size_t& occurrence = 1);
  bool        empty();
  std::size_t size();
  void        wait_for_events(std::unique_lock<std::mutex>& lock);
  BlockingQueue(const BlockingQueue& other)            = delete;
  BlockingQueue(BlockingQueue&& other)                 = delete;
  BlockingQueue& operator=(const BlockingQueue& other) = delete;
  BlockingQueue& operator=(BlockingQueue&& other)      = delete;
};

}  // namespace Private
}  // namespace Term
