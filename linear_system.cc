#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <sstream>
#include "linear_system.h"

LinearSystem::LinearSystem(Game* game) : LinearSystem(game, 0) {
}

LinearSystem::LinearSystem(Game* game, int first_entering_variable)
    : game_(game), last_leaving_variable_(-first_entering_variable) {
  iteration_ = 0;
  Initialize();
}

bool LinearSystem::IsSatisfied() {
  bool res = false;
  if (iteration_) {
    int i = 1;
    int n = game_->n(), m = game_->m();
    bool all = true;
    while (i <= n) {
      all = all && (basis_.count(i) || basis_.count(-i));
      res = res || basis_.count(i);
      i++;
    }
    if (!res || !all) {
      return false;
    }
    res = false;
    while (i <= n + m) {
      all = all && (basis_.count(i) || basis_.count(-i));
      res = res || basis_.count(i);
      i++;
    }
    if (!all) {
      return false;
    }
  }
  return res;
}

static int RandomVariable(int total) {
  srand(time(NULL));
  return (rand() % total) + 1;
}

bool LinearSystem::Iterate() {
  int n = this->game_->n(), m = this->game_->m();
  std::cout << "At iteration i = " << iteration_ << std::endl;
  std::cout << "Last leaving variable = "
            << LinearEquation::Variable(last_leaving_variable_, n, m)
            << std::endl;
  if (IsSatisfied()) {
    return true;
  } else if (!iteration_ && !last_leaving_variable_) {
    this->last_leaving_variable_ = -RandomVariable(n + m);
  } else if (iteration_) {
    int entering_variable = -this->last_leaving_variable_;
    std::cout << "Variable "
              << LinearEquation::Variable(entering_variable, n, m)
              << " enterng the basis" << std::endl;
    int clashing_variable = this->ClashingVariable();
    std::cout << "Clashed with variable "
              << LinearEquation::Variable(clashing_variable, n, m) << std::endl;
    LinearEquation clashing_equation = linear_equations_[clashing_variable];
    std::cout << "Shifting " << clashing_equation.ToString(n, m) << std::endl;
    linear_equations_.erase(clashing_variable);
    clashing_equation.ChangeLeftHandSide(entering_variable);
    std::cout << "Inserting " << clashing_equation.ToString(n, m) << std::endl;
    linear_equations_[entering_variable] = clashing_equation;
    this->basis_.erase(clashing_variable);
    this->basis_.insert(entering_variable);
    this->UpdateEquations();
    this->last_leaving_variable_ = clashing_variable;
    std::cout << this->StateToString() << std::endl;
  }
  int x = 0;
  std::cin >> x;
  iteration_++;
  return false;
}

void LinearSystem::Initialize() {
  int i = 1, n = game_->n(), m = game_->m();
  while (i <= n + m) {
    basis_.insert(-i);
    i++;
  }
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

static void Cands(const std::unordered_set<int>& s, int n, int m) {
  auto it = s.cbegin();
  while (it != s.cend()) {
    std::cout << LinearEquation::Variable(*it, n, m) << " ";
    ++it;
  }
  std::cout << std::endl;
}

int LinearSystem::ClashingVariable() {
  int next_entering_variable =
      -last_leaving_variable_;  // We have to look at the dual of
                                // |last_leaving_variable_|
  auto candidate_equations = variable_index_[next_entering_variable];
  auto it = candidate_equations.begin();
  int next_leaving_variable = 0;
  double minimum_ratio = (1 << 30);
  while (it != candidate_equations.end()) {
    double current_ratio =
        linear_equations_[*it].MinimumRatioTest(next_entering_variable);
    std::cout << "MRT of "
              << LinearEquation::Variable(*it, game_->n(), game_->m()) << " is "
              << current_ratio << std::endl;
    int tie_breaker = rand() % 2;
    if (minimum_ratio > current_ratio ||
        (minimum_ratio == current_ratio && tie_breaker)) {
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

static void ToOrderedSet(const std::unordered_map<int, LinearEquation>& eqs,
                         std::vector<std::pair<int, int> >& o_basis) {
  auto it = eqs.begin();
  while (it != eqs.end()) {
    o_basis.push_back(std::make_pair(it->second.id(), it->first));
    ++it;
  }
  std::sort(o_basis.begin(), o_basis.end());
}

std::string LinearSystem::StateToString() const {
  std::stringstream text;
  text << "Basis = {";
  std::vector<std::pair<int, int> > o_basis;
  ToOrderedSet(linear_equations_, o_basis);
  auto o_it = o_basis.begin();
  while (o_it != o_basis.end()) {
    if (o_it != o_basis.begin()) {
      text << ", ";
    }
    text << o_it->second;
    ++o_it;
  }
  text << "}" << std::endl;
  int n = this->game_->n(), m = this->game_->m();
  o_it = o_basis.begin();
  int i = 1;
  while (o_it != o_basis.end()) {
    text << linear_equations_.at(o_it->second).ToString(n, m) << std::endl;
    if (i == n) {
      text << std::endl;
    }
    i++;
    ++o_it;
  }
  text << std::endl;
  return text.str();
}

std::string LinearSystem::FinalDistribution() const {
  int n = this->game_->n(), m = this->game_->m();
  std::stringstream prob1, prob2, text;
  std::vector<double> probs(n + m + 1);
  int i = 1;
  double sum = 0;
  while (i <= n) {
    if (linear_equations_.count(i)) {
      probs[i] = linear_equations_.at(i).intercept();
      sum += linear_equations_.at(i).intercept();
    }
    i++;
  }
  i = 1;
  prob1 << "Player 1: (";
  while (i <= n) {
    if (i > 1) {
      prob1 << ", ";
    }
    probs[i] /= sum;
    prob1 << probs[i];
    i++;
  }
  prob1 << ") | Payoff = ";
  sum = 0;
  while (i <= n + m) {
    if (linear_equations_.count(i)) {
      probs[i] = linear_equations_.at(i).intercept();
      sum += linear_equations_.at(i).intercept();
    }
    i++;
  }
  i = n + 1;
  prob2 << "Player 2: (";
  while (i <= n + m) {
    if (i > n+1) {
      prob2 << ", ";
    }
    probs[i] /= sum;
    prob2 << probs[i];
    i++;
  }
  prob2 << ") | Payoff = ";
  double p1_payoff = 0, p2_payoff = 0;
  i = 0;
  while (i < n) {
    int j = 0;
    while (j < m) {
      Payoff p = this->game_->payoffs().at(i).at(j);
      p1_payoff += p.p1 * probs[i + 1] * probs[n + j + 1];
      p2_payoff += p.p2 * probs[i + 1] * probs[n + j + 1];
      j++;
    }
    i++;
  }
  prob1 << p1_payoff;
  prob2 << p2_payoff;
  text << prob1.str() << std::endl << prob2.str() << std::endl;
  return text.str();
}