#pragma once

#include <filesystem>
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
  int steps;
  std::filesystem::path configsDirectory;
  std::filesystem::path matrixFile;

  std::vector<std::string> availableModes;
  std::vector<std::string> availableStepsValues;
  std::vector<std::string> optionNames;

  void ValidateArgs();
  int CheckIfOption(const std::string &option);
  void CheckIfMode(const std::string &m);
  int IsValidMode(const std::string &m);
  void CheckIfStepsOption(const std::string &so);
  int IsValidStepsOption(const std::string &so);
  void CheckIfConfigsOption(const std::string &co);
  std::filesystem::path IsValidConfigsOption(const std::string &co);
  void CheckIfMatrixOption(const std::string &mo);
  std::filesystem::path IsValidMatrixOption(const std::string &mo);

  void ValidateStrats();
};
