#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <set>
#include <sstream>
#include <vector>
#include "linear_system.h"

LinearSystem::LinearSystem(Game* game) : LinearSystem(game, 0, false) {
}

LinearSystem::LinearSystem(Game* game, int first_entering_variable,
                           bool verbose)
    : verbose_(verbose), game_(game) {
  iteration_ = 0;
  last_leaving_variable_ =
      std::min(first_entering_variable, -first_entering_variable);
  Initialize();
}

/* static void Cands(const std::unordered_set<int>& s, int n, int m) {
  auto it = s.cbegin();
  while (it != s.cend()) {
    std::cout << LinearEquation::Variable(*it, n, m) << " ";
    ++it;
  }
  std::cout << std::endl;
}

static void PrintIndex(
    const std::unordered_map<int, std::unordered_set<int> >& index, int n,
    int m) {
  auto it = index.begin();
  while (it != index.end()) {
    std::cout << "Candidates for " << LinearEquation::Variable(it->first, n, m)
              << " are: ";
    Cands(it->second, n, m);
    ++it;
  }
} */

bool LinearSystem::BasisPlayer1(std::unordered_set<int>& p1_basis) const {
  int n = game_->n(), m = game_->m();
  std::vector<double> p1_distribution;
  int i = 1;
  while (i <= n) {
    if (!linear_equations_.count(i)) {  // x_i is not in the support.
      p1_basis.insert(i);
    }
    i++;
  }
  int j = 1;
  while (j <= m) {
    if (!linear_equations_.count(-j-n)) {  // s_j is 0'd -> y_j is a best response
      p1_basis.insert(j + n);
    }
    j++;
  }
  return true;
}

bool LinearSystem::BasisPlayer2(std::unordered_set<int>& p2_basis) const {
  int n = game_->n(), m = game_->m();
  std::vector<double> p2_distribution;
  int j = 1;
  while (j <= m) {
    if (!linear_equations_.count(j + n)) {  // y_j is not in the support
      p2_basis.insert(j + n);
    }
    j++;
  }
  int i = 1;
  while (i <= n) {
    if (!linear_equations_.count(-i)) {  // r_i is 0'd -> x_i is a best response
      p2_basis.insert(i);
    }
    i++;
  }
  return true;
}

bool LinearSystem::IsSatisfied() const {
  if (!iteration_) {
    return false;
  }
  std::unordered_set<int> p1_basis, p2_basis, basis;
  if (!BasisPlayer1(p1_basis)) {
    return false;
  }
  if (!BasisPlayer2(p2_basis)) {
    return false;
  }
  auto it = p1_basis.begin();
  while (it != p1_basis.end()) {
    basis.insert(*it);
    ++it;
  }
  it = p2_basis.begin();
  while (it != p2_basis.end()) {
    basis.insert(*it);
    ++it;
  }
  return (basis.size() == (game_->n() + game_->m()));
}

static int RandomVariable(int total) {
  srand(time(NULL));
  return (rand() % total) + 1;
}

bool LinearSystem::Iterate() {
  int n = this->game_->n(), m = this->game_->m();
  if (verbose_) {
    std::cout << "At iteration i = " << iteration_ << std::endl;
    std::cout << "Last leaving variable = "
              << LinearEquation::Variable(last_leaving_variable_, n, m)
              << std::endl;
  }
  if (IsSatisfied()) {
    return true;
  } else if (!last_leaving_variable_) {
    this->last_leaving_variable_ = -RandomVariable(n + m);
  } else {
    int entering_variable = -this->last_leaving_variable_;
    if (verbose_) {
      std::cout << "Variable "
                << LinearEquation::Variable(entering_variable, n, m)
                << " enterng the basis" << std::endl;
    }
    int clashing_variable = this->ClashingVariable();
    if (clashing_variable) {
      if (verbose_) {
        std::cout << "Clashed with variable "
                  << LinearEquation::Variable(clashing_variable, n, m)
                  << std::endl;
      }
      LinearEquation clashing_equation = linear_equations_[clashing_variable];
      if (verbose_) {
        std::cout << "Shifting " << clashing_equation.ToString(n, m)
                  << std::endl;
      }
      linear_equations_.erase(clashing_variable);
      clashing_equation.ChangeLeftHandSide(entering_variable);
      if (verbose_) {
        std::cout << "Inserting " << clashing_equation.ToString(n, m)
                  << std::endl;
      }
      linear_equations_[entering_variable] = clashing_equation;
      this->UpdateEquations();
      this->variable_index_.erase(entering_variable);
    }
    this->last_leaving_variable_ = clashing_variable;
    if (verbose_) {
      std::cout << this->StateToString() << std::endl;
    }
  }
  iteration_++;
  return false;
}

void LinearSystem::Initialize() {
  InitializePlayer1();
  InitializePlayer2();
}

void LinearSystem::InitializePlayer1() {
  const auto& payoffs = game_->payoffs();
  int n = game_->n(), m = game_->m();
  int i = 0;
  while (i < n) {
    int j = 0, key = -i - 1;
    int id = linear_equations_.size() + 1;
    linear_equations_[key] = LinearEquation(id, key, 1, &variable_index_);
    while (j < m) {
      if (payoffs.at(i).at(j).p1) {
        linear_equations_[key].AddTerm(j + n + 1, -payoffs.at(i).at(j).p1);
      }
      j++;
    }
    i++;
  }
}

void LinearSystem::InitializePlayer2() {
  const auto& payoffs = game_->payoffs();
  int n = game_->n(), m = game_->m();
  int i = 0;
  while (i < m) {
    int j = 0, key = -i - n - 1;
    int id = linear_equations_.size() + 1;
    linear_equations_[key] = LinearEquation(id, key, 1, &variable_index_);
    while (j < n) {
      if (payoffs.at(j).at(i).p2) {
        linear_equations_[key].AddTerm(j + 1, -payoffs.at(j).at(i).p2);
      }
      j++;
    }
    i++;
  }
}

int LinearSystem::ClashingVariable() const {
  int next_entering_variable =
      -last_leaving_variable_;  // We have to look at the dual of
                                // |last_leaving_variable_|
  if (!variable_index_.count(next_entering_variable)) {
    return 0;
  }
  auto candidate_equations = variable_index_.at(next_entering_variable);
  auto it = candidate_equations.begin();
  int next_leaving_variable = 0;
  double minimum_ratio = (1 << 30);
  while (it != candidate_equations.end()) {
    double current_ratio =
        linear_equations_.at(*it).MinimumRatioTest(next_entering_variable);
    if (verbose_) {
      std::cout << "MRT of "
                << LinearEquation::Variable(*it, game_->n(), game_->m())
                << " is " << current_ratio << std::endl;
    }
    int tie_breaker = rand() % 2;
    if (minimum_ratio > current_ratio ||
        (minimum_ratio == current_ratio && tie_breaker)) {
      assert(minimum_ratio != current_ratio);
      next_leaving_variable = *it;
      minimum_ratio = current_ratio;
    }
    ++it;
  }
  return next_leaving_variable;
}

void LinearSystem::UpdateEquations() {
  int entering_variable = -this->last_leaving_variable_;
  auto updating_equations = variable_index_[entering_variable];
  const auto& entering_equation = this->linear_equations_[entering_variable];
  auto it = updating_equations.begin();
  while (it != updating_equations.end()) {
    linear_equations_[*it].ReplaceVariable(entering_equation);
    ++it;
  }
}

static void ToOrderedSet(const std::unordered_set<int>& basis,
                         std::set<int>& o_basis) {
  auto it = basis.begin();
  while (it != basis.end()) {
    o_basis.insert(*it);
    ++it;
  }
}

std::string LinearSystem::StateToString() const {
  std::stringstream text;
  text << "Basis = {{";
  std::unordered_set<int> p1_basis, p2_basis;
  std::set<int> o_p1_basis, o_p2_basis;
  if (BasisPlayer1(p1_basis)) {
    ToOrderedSet(p1_basis, o_p1_basis);
  }
  if (BasisPlayer2(p2_basis)) {
    ToOrderedSet(p2_basis, o_p2_basis);
  }
  auto o_it = o_p1_basis.begin();
  while (o_it != o_p1_basis.end()) {
    if (o_it != o_p1_basis.begin()) {
      text << ", ";
    }
    text << *o_it;
    ++o_it;
  }
  text << "}, {";
  o_it = o_p2_basis.begin();
  while (o_it != o_p2_basis.end()) {
    if (o_it != o_p2_basis.begin()) {
      text << ", ";
    }
    text << *o_it;
    ++o_it;
  }
  text << "}}" << std::endl;

  int n = this->game_->n(), m = this->game_->m();
  int i = 1;
  while (i <= n + m) {
    auto eq_it = linear_equations_.cbegin();
    while (eq_it != linear_equations_.cend()) {
      if (eq_it->second.id() == i) {
        break;
      }
      ++eq_it;
    }
    text << eq_it->second.ToString(n, m) << std::endl;
    if (i == n) {
      text << std::endl;
    }
    i++;
  }
  text << std::endl;
  return text.str();
}

bool LinearSystem::PlayerDistribution(std::vector<double>& distribution,
                                      bool is_player_1) const {
  int n = this->game_->n(), m = this->game_->m();
  distribution = (is_player_1) ? std::vector<double>(n + 1, 0)
                               : std::vector<double>(m + 1, 0);
  int i = 1;
  double sum = 0;
  bool valid = false;
  while (i < distribution.size()) {
    int key = (is_player_1) ? i : i + n;
    if (this->linear_equations_.count(key)) {
      distribution[i] = linear_equations_.at(key).intercept();
      sum += distribution[i];
      valid = true;
    }
    i++;
  }
  i = 1;
  while (i < distribution.size()) {
    distribution[i] /= sum;
    i++;
  }
  return valid;
}

double LinearSystem::ExpectedPayoffs(const std::vector<double>& p1_distribution,
                                     const std::vector<double>& p2_distribution,
                                     bool is_player_1) const {
  int n = this->game_->n(), m = this->game_->m();
  double payoff = 0;
  int i = 0;
  while (i < n) {
    int j = 0;
    while (j < m) {
      double prob = p1_distribution.at(i + 1) * p2_distribution.at(j + 1);
      double utility = 0;
      if (is_player_1) {
        utility = this->game_->payoffs().at(i).at(j).p1;
      } else {
        utility = this->game_->payoffs().at(i).at(j).p2;
      }
      payoff += utility * prob;
      j++;
    }
    i++;
  }
  return payoff + game_->normalizing_payoff();
}

std::string LinearSystem::FinalDistribution() const {
  int n = this->game_->n(), m = this->game_->m();
  std::stringstream prob1, prob2, text;
  std::vector<double> p1_distribution, p2_distribution;
  this->PlayerDistribution(p1_distribution, true);
  this->PlayerDistribution(p2_distribution, false);
  double p1_payoff =
      this->ExpectedPayoffs(p1_distribution, p2_distribution, true);
  double p2_payoff =
      this->ExpectedPayoffs(p1_distribution, p2_distribution, false);
  int i = 1;
  prob1 << "Player 1: (";
  while (i <= n) {
    if (i > 1) {
      prob1 << ", ";
    }
    prob1 << p1_distribution[i];
    i++;
  }
  prob1 << ") | Payoff = " << p1_payoff;
  i = 1;
  prob2 << "Player 2: (";
  while (i <= m) {
    if (i > 1) {
      prob2 << ", ";
    }
    prob2 << p2_distribution[i];
    i++;
  }
  prob2 << ") | Payoff = " << p2_payoff;
  text << prob1.str() << std::endl << prob2.str() << std::endl;
  return text.str();
}
