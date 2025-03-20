#include "../include/BitGrid8x8.h"
#include "../include/Position.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace {
bool is_valid_pos(Position pos) {
  return pos.x >= 0 && pos.x < BitGrid8x8::MAX_X && pos.y >= 0 && pos.y < BitGrid8x8::MAX_Y;
}

/**
 * Returns the bit index for position `pos` in an 8x8 bit grid.
 *
 * Indexing begins from the right-most bit (i.e., the right-most bit has index 0, and the left-most
 * bit has index 63).
 * For example, `bit_index({0, 0}) == 63`, and `bit_index({7, 7}) == 0`.
 */
int bit_index(Position pos) {
  assert(is_valid_pos(pos));
  return (BitGrid8x8::NUM_BITS - 1) - (pos.y * BitGrid8x8::MAX_Y) - pos.x;
}
}

BitGrid8x8& BitGrid8x8::operator=(BitGrid8x8 other) {
  std::swap(m_grid_mask, other.m_grid_mask);
  return *this;
}

bool BitGrid8x8::operator==(BitGrid8x8 other) const {
  return m_grid_mask == other.m_grid_mask;
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
