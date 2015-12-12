#include <sstream>
#include "heuristic.h"

Heuristic::Heuristic(int search_depth, Game* game)
    : search_depth_(search_depth), game_(game){};

void Heuristic::Run() {
  int n = this->game_->n(), m = this->game_->m();
  int i = 1;
  while (i <= n + m) {
    RunForVariable(i);
    i++;
  }
}

int Heuristic::SelectVariable(Mode mode) {
  int variable = 0;
  Payoff p;
  auto it = this->variable_score_.begin();
  while (it != this->variable_score_.end()) {
    if (!variable) {
      variable = it->first;
      p = it->second;
    } else {
      double a = 0, b = 0;
      switch (mode) {
        case SELFISH_MAXIMIZER_1:
          a = it->second.p1;
          b = p.p1;
          break;
        case SELFISH_MAXIMIZER_2:
          a = it->second.p2;
          b = p.p2;
          break;
        case SELFLESS_MINIMIZER_1:
          a = p.p1;
          b = it->second.p1;
          break;
        case SELFLESS_MINIMIZER_2:
          a = p.p2;
          b = it->second.p2;
          break;
      }
      if (b < a) {
        variable = it->first;
        p = it->second;
      }
    }
    ++it;
  }
  return variable;
}

static bool Payoffs(const LinearSystem& system, Payoff& p) {
  std::vector<double> p1, p2;
  if (!system.PlayerDistribution(p1, true)) {
    return false;
  }
  if (!system.PlayerDistribution(p2, false)) {
    return false;
  }
  p.p1 = system.ExpectedPayoffs(p1, p2, true);
  p.p2 = system.ExpectedPayoffs(p1, p2, false);
  return true;
}

void Heuristic::RunForVariable(int variable) {
  LinearSystem system(game_, variable, false);
  while ((system.iteration() != search_depth_) && !system.Iterate()) {
  }
  Payoff p;
  if (Payoffs(system, p)) {
    variable_score_[variable] = p;
    game_states_[variable] = system.FinalDistribution();
  }
  this->game_ = system.release();
}

std::string Heuristic::ScoresToString() const {
  int n = this->game_->n(), m = this->game_->m();
  std::vector<std::pair<int, Payoff> > scores;
  auto it = this->variable_score_.cbegin();
  while (it != this->variable_score_.cend()) {
    scores.push_back(*it);
    ++it;
  }
  std::sort(scores.begin(), scores.end());
  std::stringstream text;
  auto s_it = scores.cbegin();
  while (s_it != scores.cend()) {
    text << LinearEquation::Variable(s_it->first, n, m) << ": " << std::endl
         << game_states_.at(s_it->first) << std::endl;
    s_it++;
  }
  return text.str();
}
