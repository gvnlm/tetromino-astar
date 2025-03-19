#ifndef GRID_H
#define GRID_H

#include "BitGrid.h"
#include "Position.h"
#include <cstddef>
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * Represents a 24x16 grid designed for A* search with tetromino pieces.
 *
 * Manages the grid state, which consists of the start position, the target position, obstacle
 * positions, and positions where a piece (of a tetromino) has been placed.
 * Before initialising any instances, the static member functions `set_start()`, `set_target()`,
 * `set_obstacles()` should first be called (in any order), followed by
 * `preprocess_heuristic_values()`.
 */
class Grid {
public:
  static constexpr int MAX_X{24};
  static constexpr int MAX_Y{16};
  static constexpr int TETROMINO_SIZE{4};

  static void set_start(Position pos);
  static void set_target(Position pos);
  static void set_obstacles(const BitGrid<MAX_X, MAX_Y>& obstacles);

  /**
   * Calculates the heuristic value of all non-obstacle grid positions, then stores it in
   * `s_heuristic_values`.
   *
   * The heuristic value of a position is equal to the optimal cost (in terms of tetromino moves)
   * to reach the target position. It is calculated using Dijkstra's algorithm.
   */
  static void preprocess_heuristic_values();

  /**
   * Returns `true` if there exists a path (in terms of single cell moves) from the start position
   * to the target position.
   *
   * Dependent on `preprocess_heuristic_values()` having been called beforehand.
   */
  static bool is_target_enclosed();

  static Position start();
  static Position target();
  static const BitGrid<MAX_X, MAX_Y>& obstacles();

  Grid();
  Grid(const Grid& other) = default;
  Grid& operator=(Grid other); // Pass by value to implement copy-and-swap idiom

  bool operator==(const Grid& other) const;
  bool operator<(const Grid& other) const;
  std::size_t hash() const;

  /**
   * Returns a vector containing all successor grids.
   *
   * A successor grid is the result of placing a tetromino on the calling grid, such that the
   * tetromino does not overlap with obstacles, and is adjacent to at least one already-placed
   * piece.
   */
  std::vector<Grid> successors() const;

  /**
   * Returns `true` if the target position has been reached (i.e., a piece has been placed on the
   * target position), otherwise returns `false`.
   */
  bool is_target_reached() const;

  /**
   * Calculates the difference between the calling grid and `other` grid, assuming `other` differs
   * by exactly 1 tetromino. The difference is returned as an array of 4 positions, representing a
   * tetromino.
   */
  std::array<Position, TETROMINO_SIZE> difference(const Grid& other) const;

  const BitGrid<MAX_X, MAX_Y>& placements() const;

private:
  static inline Position s_start{0, 0};
  static inline Position s_target{0, 0};
  static inline BitGrid<MAX_X, MAX_Y> s_obstacles{};
  static inline std::unordered_map<Position, int, PositionHash> s_heuristic_values{};

  // Positions where a piece has been placed
  BitGrid<MAX_X, MAX_Y> m_placements{};
  // Positions that are adjacent to at least 1 position where a piece has been placed
  BitGrid<MAX_X, MAX_Y> m_placeables{};

  // Actual cost thus far (in terms of tetromino moves)
  int m_g{0};
  // Estimated cost to target (in terms of tetromino moves)
  int m_h;

  /**
   * Places a piece at position `pos`.
   */
  void place(Position pos);

  /**
   * Returns a vector containing all successor grids that result from placing a tetromino on the
   * calling grid that overlaps position `pos`. `visited` is used for revisited-state checking.
   */
  std::vector<Grid> successors_from(
      Position pos, std::unordered_set<BitGrid<MAX_X, MAX_Y>, BitGridHash<MAX_X, MAX_Y>>& visited
  ) const;
};

struct GridHash {
  std::size_t operator()(const Grid& grid) const {
    return grid.hash();
  }
};

#endif
