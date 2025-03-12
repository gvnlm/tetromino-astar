#ifndef POSITION_H
#define POSITION_H

#include <cstddef>

struct Position {
  int x{0};
  int y{0};

  bool operator==(Position other) const {
    return x == other.x && y == other.y;
  }

  std::size_t hash() const {
    // Implement boost::hash_combine
    std::size_t seed{static_cast<std::size_t>(x)};
    seed ^= static_cast<std::size_t>(y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    return seed;
  }
};

struct PositionHash {
  std::size_t operator()(Position pos) const {
    return pos.hash();
  }
};

#endif
