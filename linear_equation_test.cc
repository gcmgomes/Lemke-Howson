#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "linear_equation.h"

using namespace std;

void print(std::unordered_map<int, std::unordered_set<int> >& index) {
  auto i = index.begin();
  while(i != index.end()) {
    cout << LinearEquation::Variable(i->first, 3, 3) << ":";
    auto j = i->second.begin();
    while(j != i->second.end()) {
      cout << " " << LinearEquation::Variable(*j, 3, 3);
      ++j;
    }
    cout << endl;
    ++i;
  }
}

int main() {
  std::unordered_map<int, std::unordered_set<int> > index;
  LinearEquation l(1, 3, 1, &index), r(2, 2, 1, &index);
  int i = 1;
  while(i <= 3) {
    l.AddTerm(-i, 2*i);
    index[-i].insert(l.lhs());
    r.AddTerm(i, -2*i);
    index[i].insert(r.lhs());
    i++;
  }
  l.AddTerm(2, 2);
  index[6].insert(l.lhs());
  print(index);
  cout << l.ToString(3, 3) << endl << r.ToString(3, 3) << endl;

  cout << endl << endl << "After replacing" << endl;
  l.ReplaceVariable(r);
  cout << l.ToString(3, 3) << endl << r.ToString(3, 3) << endl;

  print(index);

  cout << endl << endl << "After changin LHS of 1" << endl;
  l.ChangeLeftHandSide(-1);
  cout << l.ToString(3, 3) << endl << r.ToString(3, 3) << endl;

  print(index);
}
