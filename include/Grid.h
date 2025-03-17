#ifndef GRID_H
#define GRID_H

#include "BitGrid.h"
#include "Position.h"
#include <cstddef>
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Grid {
public:
  static constexpr int MAX_X{24};
  static constexpr int MAX_Y{16};
  static constexpr int TETROMINO_SIZE{4};

  static void set_start(Position pos);
  static void set_target(Position pos);
  static void set_obstacles(const BitGrid<MAX_X, MAX_Y>& obstacles);
  static void preprocess_heuristic_values();
  static bool is_target_enclosed();

  static Position start() { return s_start; }
  static Position target() { return s_target; }
  static const BitGrid<MAX_X, MAX_Y>& obstacles() { return s_obstacles; }

  Grid();
  Grid(const Grid& other);

  Grid& operator=(Grid other);
  bool operator==(const Grid& other) const;
  bool operator<(const Grid& other) const;
  friend std::ostream& operator<<(std::ostream& out, const Grid& grid);

  std::vector<Grid> successors() const;
  bool is_target_reached() const;
  std::array<Position, TETROMINO_SIZE> difference(const Grid& other) const;
  std::size_t hash() const;

  const BitGrid<MAX_X, MAX_Y>& placements() const { return m_placements; }

private:
  static inline Position s_start{0, 0};
  static inline Position s_target{0, 0};
  static inline BitGrid<MAX_X, MAX_Y> s_obstacles{};
  static inline std::unordered_map<Position, int, PositionHash> s_heuristic_values{};

  BitGrid<MAX_X, MAX_Y> m_placements{};
  BitGrid<MAX_X, MAX_Y> m_placeables{};
  int m_g{0};
  int m_h;

  void place(Position pos);
  std::vector<Grid> successors_from(
      Position pos, std::unordered_set<BitGrid<MAX_X, MAX_Y>, BitGridHash<MAX_X, MAX_Y>>& visited
  ) const;
};

struct GridHash {
  std::size_t operator()(const Grid& grid) const { return grid.hash(); }
};

#endif
