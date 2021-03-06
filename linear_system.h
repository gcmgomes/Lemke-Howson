#ifndef _LEMKE_HOWSON_LINEAR_SYSTEM_H_
#define _LEMKE_HOWSON_LINEAR_SYSTEM_H_
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "game.h"
#include "linear_equation.h"

class LinearSystem {
 public:
  // Takes ownership of |game|.
  LinearSystem(Game* game);

  // Forces the system to choose |first_entering_variable| in the
  // non-deterministic step of the first iteration. |verbose| indicates if the
  // steps of the algorithm will be displayed on stdout.
  LinearSystem(Game* game, int first_entering_variable, bool verbose);

  unsigned iteration() const {
    return iteration_;
  }

  // Relinquishes the ownership of |game_|.
  Game* release() {
    return this->game_.release();
  }

  // Returns true if the stopping conditions are satisfied, false otherwise.
  bool IsSatisfied() const;

  // Executes the next iteration of the Lemke-Howson algorithm. Each iteration
  // is marked by one variable leaving the basis. Returns true if the system has
  // found a Nash equilibrium.
  bool Iterate();

  // Returns the current state of the system (basis + equations).
  std::string StateToString() const;

  // Stores the current probability distribution for the indicated by
  // |is_player_1| in |distribution|.
  // Returns true if the distribution is valid.
  bool PlayerDistribution(std::vector<double>& distribution,
                          bool is_player_1) const;

  // Returns the expected payoffs of each player defined by |is_player_1| with
  // the probabilities of the |distribution| parameters.
  double ExpectedPayoffs(const std::vector<double>& p1_distribution,
                         const std::vector<double>& p2_distribution,
                         bool is_player_1) const;

  // Returns the vector representing the final probability distribution over the
  // players' actions, as well as the payoffs.
  std::string FinalDistribution() const;

 private:
  // Properly builds the linear system bassed on |game_|.
  void Initialize();

  // Initializes the equation set related to player 1.
  void InitializePlayer1();

  // Initializes the equation set related to player 2.
  void InitializePlayer2();

  // The basis for the row player of the game.
  bool BasisPlayer1(std::unordered_set<int>& p1_basis) const;

  // The basis for the column player of the game.
  bool BasisPlayer2(std::unordered_set<int>& p2_basis) const;

  // Returns theequation/variable that presented the smallest Minimum Ratio
  // Test.
  int ClashingVariable() const;

  // Updates every equation after a variable entered the basis.
  void UpdateEquations();

  // The iteraton that the game is currently in.
  unsigned iteration_;

  // The last variable that left the basis. The entering variable is always this
  // one's negative.
  int last_leaving_variable_;

  // If true, prints algorithm progress to stdout.
  bool verbose_;

  // The collection of linear equations that compose the system. The key is
  // the |lhs| value of the mapped equation.
  std::unordered_map<int, LinearEquation> linear_equations_;

  // A mapping from variables to equations where it exists.
  std::unordered_map<int, std::unordered_set<int> > variable_index_;

  // The game from which this linear system came from.
  std::unique_ptr<Game> game_;
};
#endif
