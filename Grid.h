#ifndef GRID_H
#define GRID_H

#include "BitGrid16x16.h"
#include "Position.h"
#include <cstddef>
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Grid {
public:
  static constexpr int MAX_X{BitGrid16x16::MAX_X};
  static constexpr int MAX_Y{BitGrid16x16::MAX_Y};

  static void set_start(Position pos);
  static void set_target(Position pos);
  static void set_obstacles(const BitGrid16x16& obstacles);
  static void preprocess_heuristic_values();

  Grid();
  Grid(const Grid& other);

  Grid& operator=(Grid other);
  bool operator==(const Grid& other) const;
  friend std::ostream& operator<<(std::ostream& out, const Grid& grid);

  std::vector<Grid> successors() const;
  std::size_t hash() const;

private:
  static inline Position s_start{0, 0};
  static inline Position s_target{0, 0};
  static inline BitGrid16x16 s_obstacles{};
  static inline std::unordered_map<Position, int, PositionHash> s_heuristic_values{};

  BitGrid16x16 m_placements{};
  BitGrid16x16 m_placeables{};

  void place(Position pos);
  std::vector<Grid> successors_from(
      Position pos, std::unordered_set<BitGrid16x16, BitGrid16x16Hash>& visited
  ) const;
};

struct GridHash {
  std::size_t operator()(const Grid& grid) const {
    return grid.hash();
  }
};

#endif
