#include <sstream>
#include "heuristic.h"

Heuristic::Heuristic(unsigned search_depth, Game* game)
    : search_depth_(search_depth), game_(game) {
  optimal_iteration_count_ = 123456789;
  minimum_payoffs_.p1 = 12345678;
  minimum_payoffs_.p2 = 12345678;
  maximum_payoffs_.p1 = -12345678;
  maximum_payoffs_.p2 = -12345678;
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

void Heuristic::Run(bool until_convergence) {
  int n = this->game_->n(), m = this->game_->m();
  int i = 1;
  while (i <= n + m) {
    RunForVariable(i, until_convergence);
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

void Heuristic::CalculateGap(Mode mode, int variable, double& iteration_gap,
                             double& payoff_gap) {
  LinearSystem system(game_, variable, false);
  while (!system.Iterate())
    ;
  Payoff p;
  Payoffs(system, p);
  iteration_gap = this->optimal_iteration_count_;
  iteration_gap = 1 - iteration_gap / (double)system.iteration();
  switch (mode) {
    case SELFISH_MAXIMIZER_1:
      payoff_gap = 1 - (p.p1 / maximum_payoffs_.p1);
      break;
    case SELFISH_MAXIMIZER_2:
      payoff_gap = 1 - (p.p2 / maximum_payoffs_.p2);
      break;
    case SELFLESS_MINIMIZER_1:
      payoff_gap = 1 - (minimum_payoffs_.p1 / p.p1);
      break;
    case SELFLESS_MINIMIZER_2:
      payoff_gap = 1 - (minimum_payoffs_.p2 / p.p2);
      break;
  }
  this->game_ = system.release();
  iteration_gap *= 100;
  payoff_gap *= 100;
}

void Heuristic::RunForVariable(int variable, bool until_convergence) {
  LinearSystem system(game_, variable, false);
  while ((system.iteration() != search_depth_ || until_convergence) &&
         !system.Iterate()) {
  }
  Payoff p;
  if (Payoffs(system, p)) {
    if (!until_convergence) {
      variable_score_[variable] = p;
      game_states_[variable] = system.FinalDistribution();
    } else {
      maximum_payoffs_.p1 = std::max(maximum_payoffs_.p1, p.p1);
      maximum_payoffs_.p2 = std::max(maximum_payoffs_.p2, p.p2);
      minimum_payoffs_.p1 = std::min(minimum_payoffs_.p1, p.p1);
      minimum_payoffs_.p2 = std::min(minimum_payoffs_.p2, p.p2);
      optimal_iteration_count_ =
          std::min(system.iteration(), optimal_iteration_count_);
    }
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
