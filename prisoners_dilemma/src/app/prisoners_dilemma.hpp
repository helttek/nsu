#pragma once

#include <string>
#include <vector>

class PrisonersDilemmaApp {
public:
  PrisonersDilemmaApp(int argc, char **argv);
  ~PrisonersDilemmaApp() = default;

  void Start();

private:
  std::vector<std::string> args;
  std::vector<std::string> stratsToPlay;
  std::string mode;

  void ValidateArgs();
  int IsValidMode(const std::string mode);
  std::string GetOptionValue(const std::string option,
                             const std::string optionName);
  void ValidateMode(const std::string mode);
};
