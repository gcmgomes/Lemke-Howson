#ifndef _LEMKE_HOWSON_HEURISTIC_H_
#define _LEMKE_HOWSON_HEURISTIC_H_
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "game.h"
#include "linear_system.h"

class Heuristic {
 public:
  enum Mode {
    SELFISH_MAXIMIZER_1,   // Tries to maximize only the payoff of player 1.
    SELFISH_MAXIMIZER_2,   // Tries to maximize only the payoff of player 2.
    SELFLESS_MINIMIZER_1,  // Tries to minimize the payoff of player 1.
    SELFLESS_MINIMIZER_2,  // Tries to minimize the payoff of player 2.
  };

  // Does not take ownership of |game|. If |search_depth| is <= 0, the algorithm
  // will run until naturally stopping for each variable in the search space.
  Heuristic(int search_depth, Game* game);

  // Explores the variable search space.
  void Run();

  // Returns the variable that has the best score according to |mode|.
  int SelectVariable(Mode mode);

  // Returns the |variable_score_| mapping in text form.
  std::string ScoresToString() const;

 private:
  // Run Lemke-Howson starting from |variable|.
  void RunForVariable(int variable);

  // |search_depth_| determines how many iterations will be carried out by
  // the Lemke-Howson algorithm before being stopped.
  int search_depth_;

  // The mapping linking a variable to the score it obtained using one of the
  // heuristics of the |Mode| enum.
  std::unordered_map<int, Payoff> variable_score_;

  // Holds the information pertaining to the state the algorithm was before
  // being interrupted.
  std::unordered_map<int, std::string> game_states_;

  // The game being solved.
  Game* game_;
};
#endif
