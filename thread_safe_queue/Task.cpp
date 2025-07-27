#include "Task.hpp"

void Task::Execute() { std::this_thread::sleep_for(std::chrono::milliseconds(timeInMilliseconds)); }