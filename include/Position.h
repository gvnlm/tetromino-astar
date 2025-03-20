#ifndef POSITION_H
#define POSITION_H

#include <cstddef>
#include <cstdint>

struct Position {
  int x{0};
  int y{0};

  bool operator==(Position other) const {
    return x == other.x && y == other.y;
  }

  std::size_t hash() const {
    // Bitwise append `y` to 'x'
    return (static_cast<int64_t>(x) << 32) | (static_cast<uint32_t>(y));
  }
};

struct PositionHash {
  std::size_t operator()(Position pos) const {
    return pos.hash();
  }
};

#endif
