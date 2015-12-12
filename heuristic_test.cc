#include <iostream>
#include <string>
#include "heuristic.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << argv[0] << " [input game]" << endl;
    return 0;
  }
  string file_path(argv[1]);
  Game g;
  g.Read(file_path);
  g.Print();
  Heuristic heuristic(10, &g);
  heuristic.Run();
  cout << heuristic.ScoresToString() << endl;
  int variable = heuristic.SelectVariable(Heuristic::Mode::SELFISH_MAXIMIZER_1);
  cout << "Best for SELFISH_MAXIMIZER_1 is "
       << LinearEquation::Variable(variable, g.n(), g.m()) << endl;
  variable = heuristic.SelectVariable(Heuristic::Mode::SELFISH_MAXIMIZER_2);
  cout << "Best for SELFISH_MAXIMIZER_2 is "
       << LinearEquation::Variable(variable, g.n(), g.m()) << endl;
  variable = heuristic.SelectVariable(Heuristic::Mode::SELFLESS_MINIMIZER_1);
  cout << "Best for SELFISH_MINIMIZER_1 is "
       << LinearEquation::Variable(variable, g.n(), g.m()) << endl;
  variable = heuristic.SelectVariable(Heuristic::Mode::SELFLESS_MINIMIZER_2);
  cout << "Best for SELFISH_MINIMIZER_2 is "
       << LinearEquation::Variable(variable, g.n(), g.m()) << endl;
}
