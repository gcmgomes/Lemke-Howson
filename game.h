#ifndef _LEMKE_HOWSON_GAME_H_
#define _LEMKE_HOWSON_GAME_H_
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

  bool operator<(const Payoff& p) const {
    if (this->p1 == p.p1) {
      return this->p2 < p.p2;
    }
    return this->p1 < p.p1;
  }
};

class Game {
 public:
  Game();

  const std::vector<std::vector<Payoff> >& payoffs() const {
    return payoffs_;
  }

  size_t n() const {
    return payoffs_.size();
  }

  size_t m() const {
    if (n()) {
      return payoffs_.front().size();
    }
    return 0;
  }

  double normalizing_payoff() const {
    return normalizing_payoff_;
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

  Payoff at(int i, int j) const;

  // Writes the contents of the game to the standard output.
  void Print();

  void Normalize();

 private:
  // Determines if normalization happened.
  bool normalized_;

  // Normalizing value to make the game non-negative.
  double normalizing_payoff_;

  // The matrix representing the game in normal form.
  std::vector<std::vector<Payoff> > payoffs_;
};
#endif
