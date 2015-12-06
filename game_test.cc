#include <iostream>
#include <string>
#include "game.h"

using namespace std;

int main(int argc, char** argv) {
  if(argc < 2) {
    cout << argv[0] << " [input game]" << endl;
    return 0;
  }
  string file_path(argv[1]);
  Game g;
  g.Read(file_path);
  g.Print();
}
