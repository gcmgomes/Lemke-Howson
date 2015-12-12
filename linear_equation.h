#ifndef _LEMKE_HOWSON_LINEAR_EQUATION_H_
#define _LEMKE_HOWSON_LINEAR_EQUATION_H_
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

class LinearEquation {
 public:
  LinearEquation(
      int id, int lhs, double intercept,
      std::unordered_map<int, std::unordered_set<int> >* variable_index)
      : id_(id), lhs_(lhs), intercept_(intercept), variable_index_(variable_index){};

  LinearEquation() : LinearEquation(0, 0, 0, NULL){};

  int id() const {
    return id_;
  }

  int lhs() const {
    return lhs_;
  }

  double intercept() const {
    return intercept_;
  }

  const std::unordered_map<int, double>& terms() const {
    return terms_;
  }

  // Adds the entry |variable| to |terms_|.
  void AddTerm(int variable, double coefficient);

  // Updates the coefficient of |variable| by na increment of
  // |coefficient_increment|.
  void IncreaseTerm(int variable, double coefficient_increment);

  // Removes the entry |variable| from |terms_|.
  void RemoveTerm(int variable);

  // Replaces |lhs_| with |new_lhs| and properly updates the coefficients.
  void ChangeLeftHandSide(int new_lhs);

  // Replaces the occurrence of |equation.lhs()| in |this|.
  void ReplaceVariable(const LinearEquation& equation);

  // Computes the minimum ratio for |entering_variable|. Used to determine the
  // clashing equation.
  double MinimumRatioTest(int entering_variable) const;

  // Converts the linear equation to text form. |n| is
  // the number of strategies for player 1, |m| for player 2.
  std::string ToString(int n, int m) const;

  // Gives the appropriate name for the variable indexed by |variable|.|n| is
  // the number of strategies for player 1, |m| for player 2.
  static std::string Variable(int variable, int n, int m);

  bool operator < (const LinearEquation& e) const {
    return this->id() < e.id();
  }

 private:
  // Returns true if |variable_index| is a NULL pointer.
  bool null() {
    return variable_index_ == NULL;
  }

  // An identification number, handy for debugging.
  int id_;

  // the variable equaled to this equation.
  int lhs_;

  // The free value of the equation.
  double intercept_;

  // Represents an equation of the form sum_i x_i * a_i. The key is the variable
  // (x_i)
  // and the mapped value is the coefficient (a_i).
  std::unordered_map<int, double> terms_;

  // Index used to speed up equation selection during a basis update. Does not
  // own the pointer.
  std::unordered_map<int, std::unordered_set<int> >* variable_index_;
};
#endif
