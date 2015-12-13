#include <iostream>
#include <string>
#include "heuristic.h"

using namespace std;

string EnumName(int i) {
  string ret_val;
  switch (i) {
    case 0:
      ret_val = "SELFISH_MAXIMIZER_1";
      break;
    case 1:
      ret_val = "SELFISH_MAXIMIZER_2";
      break;
    case 2:
      ret_val = "SELFLESS_MINIMIZER_1";
      break;
    default:
      ret_val = "SELFLESS_MINIMIZER_2";
      break;
  }
  return ret_val;
}

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
  heuristic.Run(true);
  heuristic.Run(false);
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

  int i = 0;
  while (i < 4) {
    int j = 0;
    cout << "Gaps when choosing by heuristic " << EnumName(i) << endl;
    variable = heuristic.SelectVariable((Heuristic::Mode)i);
    while (j < 4) {
      double it_g = 0, p_g = 0;
      heuristic.CalculateGap((Heuristic::Mode)j, variable, it_g, p_g);
      cout << "Against " << EnumName(j) << ": Iteration Gap = " << it_g
           << "%, Payoff Gap = " << p_g << "%" << endl;
      j++;
    }
    i++;
  }
}
