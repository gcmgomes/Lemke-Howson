#include <iostream>
#include <string>
#include "linear_system.h"

using namespace std;

int main(int argc, char** argv) {
  if(argc < 2) {
    cout << argv[0] << " [input game]" << endl;
    return 0;
  }
  string file_path(argv[1]);
  Game* g = new Game();
  g->Read(file_path);
  g->Print();
  LinearSystem system(g, 3, true);
  cout << system.StateToString() << endl;

  while(!system.Iterate());
  cout << system.FinalDistribution() << endl;
}
