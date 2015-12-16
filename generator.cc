#include<cstdlib>
#include<ctime>
#include<iostream>
#include<sstream>
#include<string>

using namespace std;

int MIN, MAX;

void print_payoff() {
  int diff = MAX - MIN + 1;
  int p1 = rand() % diff + MIN;
  int p2 = rand() % diff + MIN;

  cout << p1 << "," << p2;
}

int main(int argc, char **argv) {
  ios::sync_with_stdio(false);
  srand(time(NULL));

  if (argc < 5) {
    cerr << "Error! Parameters must be specified in the following way:\n";
    cerr << "[no. of rows] [no. of columns] [min payoff] [max payoff]" << endl;
    return 0;
  }

  stringstream ss;
  int n, m;
  ss << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << endl;
  ss >> n >> m >> MIN >> MAX;

  if (MIN > MAX) {
    int aux = MIN; MIN = MAX; MAX = aux;
    cerr << "Warning: minimum payoff > maximum payoff. Values were switched." << endl;
  }

  cout << n << " " << m << endl;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      print_payoff();
      cout << (j == m-1 ? "\n" : " ");
    }
  }
  cout.flush();
}
