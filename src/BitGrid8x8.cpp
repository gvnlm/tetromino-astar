#include "../include/BitGrid8x8.h"
#include "../include/Position.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <utility>

static bool is_valid_pos(Position pos);
static int bit_index(Position pos);

BitGrid8x8::BitGrid8x8(const BitGrid8x8& other)
    : m_grid_mask{other.m_grid_mask} {};

BitGrid8x8& BitGrid8x8::operator=(BitGrid8x8 other) {
  std::swap(m_grid_mask, other.m_grid_mask);
  return *this;
}

bool BitGrid8x8::operator==(const BitGrid8x8& other) const {
  return m_grid_mask == other.m_grid_mask;
}

std::ostream& operator<<(std::ostream& out, const BitGrid8x8& grid) {
  for (int y{0}; y < BitGrid8x8::MAX_Y; ++y) {
    for (int x{0}; x < BitGrid8x8::MAX_X; ++x) {
      out << grid.is_set({x, y}) << ' ';
    }
    out << '\n';
  }

  return out;
}

void BitGrid8x8::set(Position pos) {
  assert(is_valid_pos(pos));
  m_grid_mask |= static_cast<uint64_t>(1) << bit_index(pos);
}

void BitGrid8x8::clear(Position pos) {
  assert(is_valid_pos(pos));
  m_grid_mask &= ~(static_cast<uint64_t>(1) << bit_index(pos));
}

bool BitGrid8x8::is_set(Position pos) const {
  assert(is_valid_pos(pos));
  return (m_grid_mask >> bit_index(pos)) & 1;
}

std::size_t BitGrid8x8::hash() const {
  return m_grid_mask;
}

static bool is_valid_pos(Position pos) {
  return pos.x >= 0 && pos.x < BitGrid8x8::MAX_X && pos.y >= 0 && pos.y < BitGrid8x8::MAX_Y;
}

static int bit_index(Position pos) {
  assert(is_valid_pos(pos));
  return (BitGrid8x8::NUM_BITS - 1) - (pos.y * BitGrid8x8::MAX_Y) - pos.x;
}
