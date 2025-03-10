#ifndef GRID_H
#define GRID_H

#include "BitGrid16x16.h"
#include "Position.h"
#include <cstddef>
#include <ostream>

class Grid {
public:
  static constexpr int MAX_X{BitGrid16x16::MAX_X};
  static constexpr int MAX_Y{BitGrid16x16::MAX_Y};

  static void set_start(Position pos);
  static void set_target(Position pos);
  static void set_obstacles(const BitGrid16x16& obstacles);

  Grid();
  Grid(const Grid& other);

  Grid& operator=(Grid other);
  bool operator==(const Grid& other) const;
  friend std::ostream& operator<<(std::ostream& out, const Grid& grid);

  std::size_t hash() const;

private:
  static inline Position s_start{0, 0};
  static inline Position s_target{0, 0};
  static inline BitGrid16x16 s_obstacles{};

  BitGrid16x16 m_placements{};
  BitGrid16x16 m_placeables{};

  void place(Position pos);
};

struct GridHash {
  std::size_t operator()(const Grid& grid) const {
    return grid.hash();
  }
};

#endif
