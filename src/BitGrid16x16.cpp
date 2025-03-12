#include "../include/BitGrid16x16.h"
#include "../include/BitGrid8x8.h"
#include "../include/Position.h"
#include <cassert>
#include <cstddef>
#include <ostream>
#include <utility>
#include <vector>

static bool is_valid_pos(Position pos);
static Position relative_quadrant_pos(Position pos);

BitGrid16x16::BitGrid16x16(const std::vector<Position>& positions_to_set) {
  assert(positions_to_set.size() < NUM_BITS);

  for (const auto& pos : positions_to_set) {
    assert(is_valid_pos(pos));
    set(pos);
  }
}

BitGrid16x16::BitGrid16x16(const BitGrid16x16& other) {
  for (int v{0}; v < max_vertical_pos; ++v) {
    for (int h{0}; h < max_horizontal_pos; ++h) {
      m_quadrants[v][h] = other.m_quadrants[v][h];
    }
  }
}

BitGrid16x16& BitGrid16x16::operator=(BitGrid16x16 other) {
  std::swap(m_quadrants, other.m_quadrants);
  return *this;
}

bool BitGrid16x16::operator==(const BitGrid16x16& other) const {
  for (int v{0}; v < max_vertical_pos; ++v) {
    for (int h{0}; h < max_horizontal_pos; ++h) {
      if (m_quadrants[v][h] != other.m_quadrants[v][h]) {
        return false;
      }
    }
  }

  return true;
}

std::ostream& operator<<(std::ostream& out, const BitGrid16x16& grid) {
  for (int y{0}; y < BitGrid16x16::MAX_Y; ++y) {
    for (int x{0}; x < BitGrid16x16::MAX_X; ++x) {
      out << grid.is_set({x, y}) << ' ';
    }
    out << '\n';
  }

  return out;
}

void BitGrid16x16::set(Position pos) {
  assert(is_valid_pos(pos));
  quadrant(pos).set(relative_quadrant_pos(pos));
}

void BitGrid16x16::clear(Position pos) {
  assert(is_valid_pos(pos));
  quadrant(pos).clear(relative_quadrant_pos(pos));
}

bool BitGrid16x16::is_set(Position pos) const {
  assert(is_valid_pos(pos));
  return quadrant(pos).is_set(relative_quadrant_pos(pos));
}

std::size_t BitGrid16x16::hash() const {
  // Implement boost::hash_combine
  std::size_t seed{m_quadrants[top][left].hash()};
  seed ^= m_quadrants[top][right].hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= m_quadrants[bottom][left].hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= m_quadrants[bottom][right].hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);

  return seed;
}

BitGrid8x8& BitGrid16x16::quadrant(Position pos) {
  assert(is_valid_pos(pos));

  int vertical_pos{(pos.y < BitGrid8x8::MAX_Y) ? top : bottom};
  int horizontal_pos{(pos.x < BitGrid8x8::MAX_X) ? left : right};

  return m_quadrants[vertical_pos][horizontal_pos];
}

const BitGrid8x8& BitGrid16x16::quadrant(Position pos) const {
  assert(is_valid_pos(pos));

  int vertical_pos{(pos.y < BitGrid8x8::MAX_Y) ? top : bottom};
  int horizontal_pos{(pos.x < BitGrid8x8::MAX_X) ? left : right};

  return m_quadrants[vertical_pos][horizontal_pos];
}

static bool is_valid_pos(Position pos) {
  return pos.x >= 0 && pos.x < BitGrid16x16::MAX_X && pos.y >= 0 && pos.y < BitGrid16x16::MAX_Y;
}

static Position relative_quadrant_pos(Position pos) {
  assert(is_valid_pos(pos));
  return {pos.x % BitGrid8x8::MAX_X, pos.y % BitGrid8x8::MAX_Y};
}
