#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

struct Payoff {
  Payoff(double a, double b) : p1(a), p2(b){};
  Payoff() : Payoff(0, 0){};
  double p1;
  double p2;
};

class Game {
 public:
  Game(){};

  const std::vector<std::vector<Payoff> >& payoffs() const {
    return payoffs_;
  }

  size_t n() const {
    return payoffs_.size();
  }

  size_t m() const {
    if(n()) {
      return payoffs_.front().size();
    }
    return 0;
  }

  // Reads the game from the file pointed by |file_path|.
  // Let N be the number of lines and M of columns. The file format should be as
  // follows:
  // N M
  // X_(1,1), Y_(1,1) ...  X(1,M), Y_(1,M)
  //
  // X_(N,1), Y_(N,1) ...  X(N,M), Y_(N,M)
  // With X_(i, j) the payoff for player 1 when player 1 plays A_i and player 2
  // players B_j. Analogously for Y_(i,j).
  void Read(const std::string& file_path);

  // Writes the contents of the game to the standard output.
  void Print();

 private:
  // The matrix representing the game in normal form.
  std::vector<std::vector<Payoff> > payoffs_;
};
