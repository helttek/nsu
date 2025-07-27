#include "prisoners_dilemma.hpp"

#include <iostream>

int main(int argc, char **argv)
{
  try
  {
    PrisonersDilemmaApp prisonersDilemma(argc, argv);
    prisonersDilemma.Start();
  }
  catch (const std::runtime_error &e)
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
