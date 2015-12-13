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
    SELFISH_MAXIMIZER_1 = 0,   // Tries to maximize only the payoff of player 1.
    SELFISH_MAXIMIZER_2 = 1,   // Tries to maximize only the payoff of player 2.
    SELFLESS_MINIMIZER_1 = 2,  // Tries to minimize the payoff of player 1.
    SELFLESS_MINIMIZER_2 = 3,  // Tries to minimize the payoff of player 2.
  };

  // Does not take ownership of |game|.
  Heuristic(unsigned search_depth, Game* game);

  // Explores the variable search space. If |until_convergence| is set, let the
  // algorithm terminate normally and register the optimal outcome for three
  // criteria:
  // 1) Number of iterations to until convergence;
  // 2) Maximum payoff for each player.
  // 3) Minimum payoff for each player.
  void Run(bool until_convergence);

  // Returns the variable that has the best score according to |mode|.
  int SelectVariable(Mode mode);

  // Calculates the gap of the heuristic defined by |mode| when starting from
  // |variable|. Stores the percentual gaps in |iteration_gap| and |payoff_gap|.
  void CalculateGap(Mode mode, int variable, double& iteration_gap,
                    double& payoff_gap);

  // Returns the |variable_score_| mapping in text form.
  std::string ScoresToString() const;

 private:
  // Run Lemke-Howson starting from |variable|. If |until_convergence| is true,
  // let the algorithm run until it naturally stops, otherwise end after
  // |search_depth_| iterations.
  void RunForVariable(int variable, bool until_convergence);

  // |search_depth_| determines how many iterations will be carried out by
  // the Lemke-Howson algorithm before being stopped.
  unsigned search_depth_;

  // The optimal number of iterations needed to find a Nash equilibrium.
  unsigned optimal_iteration_count_;

  // The best payoffs achievable by each player.
  Payoff maximum_payoffs_;

  // The worst payoffs achievable by each player.
  Payoff minimum_payoffs_;

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
