#ifndef BIT_GRID_8X8_H
#define BIT_GRID_8X8_H

#include "Position.h"
#include <cstddef>
#include <cstdint>

/**
 * Represents an 8x8 bit grid.
 *
 * Provides member functions for setting, clearing, and checking bits based on a position system.
 * The top-left corner bit has position (x=0, y=0), and the bottom-right corner bit has position
 * (x=7, y=7).
 */
class BitGrid8x8 {
public:
  static constexpr int MAX_X{8};
  static constexpr int MAX_Y{8};
  static constexpr int NUM_BITS{64};

  BitGrid8x8() = default;
  BitGrid8x8(const BitGrid8x8& other) = default;
  BitGrid8x8& operator=(BitGrid8x8 other);

  bool operator==(BitGrid8x8 other) const;
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
  uint64_t m_grid_mask{0};
};

#endif
