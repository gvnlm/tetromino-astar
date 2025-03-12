#ifndef BIT_GRID_16X16_H
#define BIT_GRID_16X16_H

#include "BitGrid8x8.h"
#include "Position.h"
#include <cstddef>
#include <ostream>
#include <vector>

class BitGrid16x16 {
public:
  static constexpr int MAX_X{16};
  static constexpr int MAX_Y{16};
  static constexpr int NUM_BITS{MAX_X * MAX_Y};

  BitGrid16x16() = default;
  BitGrid16x16(const std::vector<Position>& positions_to_set);
  BitGrid16x16(const BitGrid16x16& other);

  BitGrid16x16& operator=(BitGrid16x16 other);
  bool operator==(const BitGrid16x16& other) const;
  friend std::ostream& operator<<(std::ostream& out, const BitGrid16x16& grid);

  void set(Position pos);
  void clear(Position pos);
  bool is_set(Position pos) const;
  std::size_t hash() const;

private:
  enum VerticalPos {
    top,
    bottom,
    max_vertical_pos
  };

  enum HorizontalPos {
    left,
    right,
    max_horizontal_pos
  };

  BitGrid8x8 m_quadrants[max_vertical_pos][max_horizontal_pos]{};

  BitGrid8x8& quadrant(Position pos);
  const BitGrid8x8& quadrant(Position pos) const;
};

struct BitGrid16x16Hash {
  std::size_t operator()(const BitGrid16x16& grid) const {
    return grid.hash();
  }
};

#endif
