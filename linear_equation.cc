#include <sstream>
#include "linear_equation.h"

void LinearEquation::AddTerm(int variable, double coefficient) {
  terms_[variable] = coefficient;
  if (!null()) {
    (*variable_index_)[variable].insert(lhs_);
  }
}

void LinearEquation::IncreaseTerm(int variable, double coefficient_increment) {
  terms_[variable] += coefficient_increment;
  if (!null()) {
    (*variable_index_)[variable].insert(lhs_);
  }
}

void LinearEquation::RemoveTerm(int variable) {
  if (!terms_.count(variable)) {
    return;
  }
  if (!null()) {
    (*variable_index_)[variable].erase(lhs_);
    if((*variable_index_)[variable].empty()) {
      (*variable_index_).erase(variable);
    }
  }
  terms_.erase(variable);
}

void LinearEquation::ChangeLeftHandSide(int new_lhs) {
  if (!terms_.count(new_lhs)) {
    return;
  }
  double dividing_coefficient = -terms_[new_lhs];
  intercept_ /= dividing_coefficient;
  auto it = terms_.begin();
  while (it != terms_.end()) {
    it->second /= dividing_coefficient;
    if (!null()) {
      // We have to update the index. Effectively, we are replacing |lhs_| with
      // |new_lhs|.
      (*variable_index_)[it->first].erase(lhs_);
      (*variable_index_)[it->first].insert(new_lhs);
    }
    ++it;
  }
  this->RemoveTerm(new_lhs);
  int old_lhs = lhs_;
  lhs_ = new_lhs;
  this->AddTerm(old_lhs, -1 / dividing_coefficient);
}

void LinearEquation::ReplaceVariable(const LinearEquation& equation) {
  int e_lhs = equation.lhs();
  if (!this->terms_.count(e_lhs)) {
    return;
  }
  double coefficient = this->terms_[e_lhs];
  const auto& terms = equation.terms();
  auto it = terms.begin();
  while (it != terms.end()) {
    int e_variable = it->first;
    double e_coefficient = it->second;
    this->IncreaseTerm(e_variable, coefficient * e_coefficient);
    if (!this->terms_[e_variable]) {
      this->RemoveTerm(e_variable);
    }
    ++it;
  }
  double e_intercept = equation.intercept();
  this->intercept_ += coefficient * e_intercept;
  this->RemoveTerm(e_lhs);
}

double LinearEquation::MinimumRatioTest(int entering_variable) const {
  if (!this->terms_.count(entering_variable)) {
    return 1 << 31;
  }
  return this->terms_.at(entering_variable) / this->intercept_;
}

std::string LinearEquation::Variable(int variable, int n, int m) {
  std::stringstream text;
  std::string temp;
  if(variable > 0) {
    if(variable <= n) {
      temp = "x";
    }
    else {
      temp = "y";
    }
  }
  else {
    if(abs(variable) <= n) {
      temp = "r";
    }
    else {
      temp = "s";
    }
  }
  temp += "_";  
  text << temp << abs(variable);
  return text.str();
}

std::string LinearEquation::ToString(int n, int m) const {
  std::stringstream text;
  text << id_ << ": " << Variable(lhs_, n, m) << " = " << intercept_;
  auto it = terms_.cbegin();
  while (it != terms_.cend()) {
    text << " + " << it->second << "*" << Variable(it->first, n, m);
    ++it;
  }
  return text.str();
}
