#pragma once

#include <stdexcept>

class NotAnOption : public std::runtime_error {
public:
  explicit NotAnOption(const std::string &message)
      : std::runtime_error(message) {}
};

class InvalidOption : public std::runtime_error {
public:
  explicit InvalidOption(const std::string &message)
      : std::runtime_error(message) {}
};