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
  // non-deterministic step of the first iteration;
  LinearSystem(Game* game, int first_entering_variable);

  // Returns true if the stopping conditions are satisfied, false otherwise.
  bool IsSatisfied();

  // Executes the next iteration of the Lemke-Howson algorithm. Each iteration
  // is marked by one variable leaving the basis.
  bool Iterate();

  // Returns the current state of the system (basis + equations).
  std::string StateToString() const;

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

  // Returns theequation/variable that presented the smallest Minimum Ratio
  // Test.
  int ClashingVariable();

  // Updates every equation after a variable entered the basis.
  void UpdateEquations();

  // The iteraton that the game is currently in.
  unsigned iteration_;

  // The last variable that left the basis. The entering variable is always this
  // one's negative.
  int last_leaving_variable_;

  // The indices of the variables currently in the basis.
  std::unordered_set<int> basis_;

  // The collection of linear equations that compose the system. The key should
  // be the |lhs| value of the mapped equation.
  std::unordered_map<int, LinearEquation> linear_equations_;

  // A mapping from variables to equations where it exists.
  std::unordered_map<int, std::unordered_set<int> > variable_index_;

  // The game from which this linear system came from.
  std::unique_ptr<Game> game_;
};
