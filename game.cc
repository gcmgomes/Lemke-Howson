#include <fstream>
#include "game.h"

Game::Game() {
  normalizing_payoff_ = 123456789;
  normalized_ = false;
}

void Game::Read(const std::string& file_path) {
  std::ifstream file(file_path.c_str());
  int n = 0, m = 0;
  file >> n >> m;
  payoffs_.assign(n, std::vector<Payoff>(m));
  int i = 0;
  while (i < n) {
    int j = 0;
    while (j < m) {
      double a = 0, b = 0;
      char c = 0;
      file >> a >> c >> b;
      payoffs_[i][j] = Payoff(a, b);
      normalizing_payoff_ = std::min(normalizing_payoff_, std::min(a, b));
      j++;
    }
    i++;
  }
  Normalize();
}

void Game::Normalize() {
  int i = 0;
  while(i < n()) {
    int j = 0;
    while(j < m()) {
      payoffs_[i][j].p1 -= normalizing_payoff_;
      payoffs_[i][j].p2 -= normalizing_payoff_;
      j++;
    }
    i++;
  }
  normalized_ = true;
}
Payoff Game::at(int i, int j) const {
  i = std::max(1, i);
  j = std::max(1, j);
  i = std::min(i, (int)n());
  j = std::min(j, (int)m());
  // std::cout << "Checking " << i << ", " << j << std::endl;
  Payoff val = payoffs_.at(i - 1).at(j - 1);
  // std::cout << "Checked " << i << ", " << j << std::endl;
  return val;
}

void Game::Print() {
  int n = payoffs_.size(), m = payoffs_.front().size();
  std::cout << n << " " << m << std::endl;
  int i = 0;
  while (i < n) {
    int j = 0;
    while (j < m) {
      if (j) {
        std::cout << ' ';
      }
      std::cout << payoffs_[i][j].p1 << "," << payoffs_[i][j].p2;
      j++;
    }
    std::cout << std::endl;
    i++;
  }
}
