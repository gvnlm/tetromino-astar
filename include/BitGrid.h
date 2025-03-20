#ifndef BIT_GRID_H
#define BIT_GRID_H

#include "BitGrid8x8.h"
#include "Position.h"
#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <limits>
#include <random>
#include <vector>

namespace {
template <int Width, int Height>
std::array<std::array<std::size_t, Height>, Width> make_zobrist_table() {
  std::mt19937_64 rng{};
  std::uniform_int_distribution<std::size_t> distribution{
      std::numeric_limits<std::size_t>::min(), std::numeric_limits<std::size_t>::max()
  };

  std::array<std::array<std::size_t, Height>, Width> zobrist_table{};

  for (int x{0}; x < Width; ++x) {
    for (int y{0}; y < Height; ++y) {
      zobrist_table[x][y] = distribution(rng);
    }
  }

  return zobrist_table;
};
}

/**
 * Represents an arbitrarily sized bit grid.
 *
 * Provides member functions for setting, clearing, and checking bits based on a position system.
 * The top-left corner bit has position (x=0, y=0), and the bottom-right corner bit has position
 * (x=Width-1, y=Height-1).
 * Implemented using 8x8 bit grids, thus dimensions that are multiples of 8 are more memory
 * efficient, since all bits are utilised.
 */
template <int Width, int Height>
class BitGrid {
  static_assert(Width > 0);
  static_assert(Height > 0);

public:
  static constexpr int MAX_X{Width};
  static constexpr int MAX_Y{Height};
  static constexpr int NUM_CELLS{Width * Height};

  BitGrid() = default;
  BitGrid(const std::vector<Position>& positions_to_set);
  BitGrid(const BitGrid& other) = default;
  BitGrid& operator=(BitGrid other); // Pass by value to implement copy-and-swap idiom

  bool operator==(const BitGrid& other) const;
  std::size_t hash() const;

  /**
   * Sets the bit at position `pos` to 1.
   */
  void set(Position pos);

  /**
   * Clears the bit at position `pos` (i.e., sets it to 0).
   */
  void clear(Position pos);

  /**
   * Returns `true` if the bit at position `pos` is set (i.e., is 1), otherwise returns `false`.
   */
  bool is_set(Position pos) const;

private:
  // Number of 8x8 subgrids along x-axis
  static constexpr int MAX_SUBGRID_X{(Width + (BitGrid8x8::MAX_X - 1)) / BitGrid8x8::MAX_X};
  // Number of 8x8 subgrids along y-axis
  static constexpr int MAX_SUBGRID_Y{(Height + (BitGrid8x8::MAX_Y - 1)) / BitGrid8x8::MAX_Y};
  static inline std::array<std::array<std::size_t, Height>, Width> zobrist_table{
      make_zobrist_table<Width, Height>()
  };

  // Implement bit grid as a collection of 8x8 subgrids
  BitGrid8x8 m_subgrids[MAX_SUBGRID_X][MAX_SUBGRID_Y]{};
  std::size_t zobrist_hash{0};

  bool is_valid_pos(Position pos) const;

  /**
   * Returns a reference to the 8x8 subgrid that contains position `pos`.
   *
   * For example, `subgrid_at({0, 0})` returns the top-left corner 8x8 subgrid, since position
   * (x=0, y=0) is contained in the top-left corner 8x8 subgrid.
   */
  BitGrid8x8& subgrid_at(Position pos);

  /**
   * Returns a const reference to the 8x8 subgrid that contains position `pos`.
   *
   * For example, `subgrid_at({0, 0})` returns the top-left corner 8x8 subgrid, since position
   * (x=0, y=0) is contained in the top-left corner 8x8 subgrid.
   */
  const BitGrid8x8& subgrid_at(Position pos) const;

  /**
   * Returns the position of position `pos` relative to its corresponding 8x8 subgrid.
   *
   * For example, `relative_subgrid_pos({8, 0}) == {0, 0}`, since within position (x=8, y=0)'s
   * corresponding subgrid (i.e., the second subgrid in the first row), its relative position is
   * (x=0, y=0).
   */
  Position relative_subgrid_pos(Position pos) const;
};

template <int Width, int Height>
struct BitGridHash {
  std::size_t operator()(const BitGrid<Width, Height>& grid) const {
    return grid.hash();
  }
};

template <int Width, int Height>
BitGrid<Width, Height>::BitGrid(const std::vector<Position>& positions_to_set) {
  assert(positions_to_set.size() < NUM_CELLS); // Assumes no duplicate positions

  for (auto pos : positions_to_set) {
    assert(is_valid_pos(pos));
    set(pos);
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
std::size_t BitGrid<Width, Height>::hash() const {
  return zobrist_hash;
}

template <int Width, int Height>
void BitGrid<Width, Height>::set(Position pos) {
  assert(is_valid_pos(pos));
  subgrid_at(pos).set(relative_subgrid_pos(pos));
  zobrist_hash ^= zobrist_table[pos.x][pos.y];
}

template <int Width, int Height>
void BitGrid<Width, Height>::clear(Position pos) {
  assert(is_valid_pos(pos));
  subgrid_at(pos).clear(relative_subgrid_pos(pos));
  zobrist_hash ^= zobrist_table[pos.x][pos.y];
}

template <int Width, int Height>
bool BitGrid<Width, Height>::is_set(Position pos) const {
  assert(is_valid_pos(pos));
  return subgrid_at(pos).is_set(relative_subgrid_pos(pos));
}

template <int Width, int Height>
bool BitGrid<Width, Height>::is_valid_pos(Position pos) const {
  return pos.x >= 0 && pos.x < Width && pos.y >= 0 && pos.y < Height;
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

template <int Width, int Height>
Position BitGrid<Width, Height>::relative_subgrid_pos(Position pos) const {
  assert(is_valid_pos(pos));
  return {pos.x % BitGrid8x8::MAX_X, pos.y % BitGrid8x8::MAX_Y};
}

#endif
