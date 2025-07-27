#pragma once
#include <thread>

class Task
{
private:
public:
  Task() = default;
  Task(int timeInMillisec) : timeInMilliseconds(timeInMillisec) {}
  ~Task() = default;
  void Execute();
  int timeInMilliseconds;
};
