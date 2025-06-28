#pragma once

#include <stdexcept>

class NotAnOption : public std::runtime_error
{
public:
  explicit NotAnOption(const std::string &message)
      : std::runtime_error(message) {}
};

class InvalidOption : public std::invalid_argument
{
public:
  explicit InvalidOption(const std::string &message)
      : std::invalid_argument(message) {}
};

class NotAStrat : public std::invalid_argument
{
public:
  explicit NotAStrat(const std::string &message)
      : std::invalid_argument(message) {}
};