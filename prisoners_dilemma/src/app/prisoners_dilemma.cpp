#include "prisoners_dilemma.hpp"
#include "../utils/include/exceptions.hpp"

#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>

PrisonersDilemmaApp::PrisonersDilemmaApp(int argc, char **argv)
    : args(argv, argv + argc), stratsToPlay(6) {
  ValidateArgs();
}

void PrisonersDilemmaApp::Start() {}

void PrisonersDilemmaApp::ValidateArgs() {
  for (auto arg : this->args) {
    std::cout << arg << std::endl;

    try {
      ValidateMode(GetOptionValue(arg, "mode"));
    } catch (NotAnOption e) {
    } catch (InvalidOption e) {
      std::cout << "Failed to validate option: " << e.what() << std::endl;
    }

    stratsToPlay.push_back(arg);
  }
}

std::string PrisonersDilemmaApp::GetOptionValue(const std::string option,
                                                const std::string optionName) {
  size_t pos = option.find("--" + optionName + "=");
  if (pos == std::string::npos) {
    throw NotAnOption("not an option");
  }
  if (!pos) {
    throw NotAnOption("not an option");
  }
  if (pos >= option.size()) {
    throw InvalidOption("invalid option value - ");
  }
  return std::string(option.substr(pos + 1)); // copy elision
}

void PrisonersDilemmaApp::ValidateMode(const std::string mode) {
  if (!IsValidMode(mode)) {
    throw InvalidOption("invalid mode value - " + mode);
  }
  this->mode = mode;
}

int PrisonersDilemmaApp::IsValidMode(const std::string mode) { return 1; }