#ifndef BIT_GRID_8X8_H
#define BIT_GRID_8X8_H

#include "Position.h"
#include <cstddef>
#include <cstdint>
#include <ostream>

class BitGrid8x8 {
public:
  static constexpr int MAX_X{8};
  static constexpr int MAX_Y{8};
  static constexpr int NUM_BITS{MAX_X * MAX_Y};

  BitGrid8x8() = default;
  BitGrid8x8(const BitGrid8x8& other);

  BitGrid8x8& operator=(BitGrid8x8 other);
  bool operator==(const BitGrid8x8& other) const;
  friend std::ostream& operator<<(std::ostream& out, const BitGrid8x8& grid);

  void set(Position pos);
  void clear(Position pos);
  bool is_set(Position pos) const;
  std::size_t hash() const;

private:
  uint64_t m_grid_mask{0};
};

#endif
