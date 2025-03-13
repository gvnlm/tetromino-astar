#ifndef BIT_GRID_H
#define BIT_GRID_H

#include "BitGrid8x8.h"
#include "Position.h"
#include <cassert>
#include <cstddef>
#include <ostream>
#include <vector>

template <int Width, int Height>
class BitGrid {
  static_assert(Width > 0 && "Grid width must be greater than 0");
  static_assert(Height > 0 && "Grid width must be greater than 0");

public:
  static constexpr int MAX_X{Width};
  static constexpr int MAX_Y{Height};
  static constexpr int NUM_CELLS{MAX_X * MAX_Y};

  BitGrid() = default;
  BitGrid(const std::vector<Position>& positions_to_set);
  BitGrid(const BitGrid& other);

  BitGrid& operator=(BitGrid other);
  bool operator==(const BitGrid& other) const;
  friend std::ostream& operator<<(std::ostream& out, const BitGrid& grid);

  void set(Position pos);
  void clear(Position pos);
  bool is_set(Position pos) const;
  std::size_t hash() const;

private:
  // Number of 8x8 subgrids along the x-axis
  static constexpr int MAX_SUBGRID_X{(Width + (BitGrid8x8::MAX_X - 1)) / BitGrid8x8::MAX_X};
  // Number of 8x8 subgrids along the y-axis
  static constexpr int MAX_SUBGRID_Y{(Height + (BitGrid8x8::MAX_Y - 1)) / BitGrid8x8::MAX_Y};

  BitGrid8x8 m_subgrids[MAX_SUBGRID_X][MAX_SUBGRID_Y]{};

  bool is_valid_pos(Position pos) const;
  Position relative_subgrid_pos(Position pos) const;
  BitGrid8x8& subgrid_at(Position pos);
  const BitGrid8x8& subgrid_at(Position pos) const;
};

template <int Width, int Height>
struct BitGridHash {
  std::size_t operator()(const BitGrid<Width, Height>& grid) const {
    return grid.hash();
  }
};

template <int Width, int Height>
BitGrid<Width, Height>::BitGrid(const std::vector<Position>& positions_to_set) {
  assert(positions_to_set.size() < NUM_CELLS);

  for (const auto& pos : positions_to_set) {
    assert(is_valid_pos(pos));
    set(pos);
  }
}

template <int Width, int Height>
BitGrid<Width, Height>::BitGrid(const BitGrid& other) {
  for (int i{0}; i < MAX_SUBGRID_X; ++i) {
    for (int j{0}; j < MAX_SUBGRID_Y; ++j) {
      m_subgrids[i][j] = other.m_subgrids[i][j];
    }
  }
}

template <int Width, int Height>
BitGrid<Width, Height>& BitGrid<Width, Height>::operator=(BitGrid other) {
  std::swap(m_subgrids, other.m_subgrids);
  return *this;
}

template <int Width, int Height>
bool BitGrid<Width, Height>::operator==(const BitGrid& other) const {
  for (int i{0}; i < MAX_SUBGRID_X; ++i) {
    for (int j{0}; j < MAX_SUBGRID_Y; ++j) {
      if (m_subgrids[i][j] != other.m_subgrids[i][j]) {
        return false;
      }
    }
  }

  return true;
}

template <int Width, int Height>
std::ostream& operator<<(std::ostream& out, const BitGrid<Width, Height>& grid) {
  for (int y{0}; y < BitGrid<Width, Height>::MAX_Y; ++y) {
    for (int x{0}; x < BitGrid<Width, Height>::MAX_X; ++x) {
      out << grid.is_set({x, y}) << ' ';
    }
    out << '\n';
  }

  return out;
}

template <int Width, int Height>
void BitGrid<Width, Height>::set(Position pos) {
  assert(is_valid_pos(pos));
  subgrid_at(pos).set(relative_subgrid_pos(pos));
}

template <int Width, int Height>
void BitGrid<Width, Height>::clear(Position pos) {
  assert(is_valid_pos(pos));
  subgrid_at(pos).clear(relative_subgrid_pos(pos));
}

template <int Width, int Height>
bool BitGrid<Width, Height>::is_set(Position pos) const {
  assert(is_valid_pos(pos));
  return subgrid_at(pos).is_set(relative_subgrid_pos(pos));
}

template <int Width, int Height>
std::size_t BitGrid<Width, Height>::hash() const {
  // Implement boost::hash_combine
  std::size_t seed{m_subgrids[0][0].hash()};
  for (int i{0}; i < MAX_SUBGRID_X; ++i) {
    for (int j{0}; j < MAX_SUBGRID_Y; ++j) {
      if (i != 0 && j != 0) {
        seed ^= m_subgrids[i][j].hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      }
    }
  }

  return seed;
}

template <int Width, int Height>
bool BitGrid<Width, Height>::is_valid_pos(Position pos) const {
  return pos.x >= 0 && pos.x < BitGrid<Width, Height>::MAX_X && pos.y >= 0
         && pos.y < BitGrid<Width, Height>::MAX_Y;
}

template <int Width, int Height>
Position BitGrid<Width, Height>::relative_subgrid_pos(Position pos) const {
  assert(is_valid_pos(pos));
  return {pos.x % BitGrid8x8::MAX_X, pos.y % BitGrid8x8::MAX_Y};
}

template <int Width, int Height>
BitGrid8x8& BitGrid<Width, Height>::subgrid_at(Position pos) {
  assert(is_valid_pos(pos));
  return m_subgrids[pos.x / BitGrid8x8::MAX_X][pos.y / BitGrid8x8::MAX_Y];
}

template <int Width, int Height>
const BitGrid8x8& BitGrid<Width, Height>::subgrid_at(Position pos) const {
  assert(is_valid_pos(pos));
  return m_subgrids[pos.x / BitGrid8x8::MAX_X][pos.y / BitGrid8x8::MAX_Y];
}

#endif
