#include <fstream>
#include "game.h"

void Game::Read(const std::string& file_path) {
  std::ifstream file(file_path.c_str());
  int n = 0, m = 0;
  file >> n >> m;
  payoffs_.assign(n, std::vector<Payoff>(m));
  int i = 0;
  while(i < n) {
    int j = 0;
    while(j < m) {
      double a = 0, b = 0;
      char c = 0;
      file >> a >> c >> b;
      payoffs_[i][j] = Payoff(a, b);
      j++;
    }
    i++;
  }
}

void Game::Print() {
  int n = payoffs_.size(), m = payoffs_.front().size();
  std::cout << n << " " << m << std::endl;
  int i = 0;
  while(i < n) {
    int j = 0;
    while(j < m) {
      if(j) {
        std::cout << ' ';
      }
      std::cout << payoffs_[i][j].p1 << "," << payoffs_[i][j].p2;
      j++;
    }
    std::cout << std::endl;
    i++;
  }
}
