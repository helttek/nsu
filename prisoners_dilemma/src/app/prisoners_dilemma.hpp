#pragma once

#include <filesystem>
#include <set>
#include <string>
#include <vector>

class PrisonersDilemmaApp
{
public:
  PrisonersDilemmaApp(int argc, char **argv);
  ~PrisonersDilemmaApp() = default;

  void Start();

private:
  std::vector<std::string> args;

  // provided option values
  std::set<std::string> stratsToPlay;
  std::string mode;
  int steps;
  std::filesystem::path configsDirectory;
  std::filesystem::path matrixFile;

  // options limits
  std::set<std::string> availableModes;
  std::set<std::string> strategies;

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
  void ValidateStrat(const std::string &strat);

  void ValidateStratsFromConfigsDir();
  void LoadRules();

  void LoadStrategisFromConfigsDirectory();

  void LoadRulesFromFile();
  void LoadDefaultRules();
};
