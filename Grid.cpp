#include "Grid.h"
#include "BitGrid16x16.h"
#include "Position.h"
#include <cassert>
#include <cstddef>
#include <ostream>
#include <utility>

static bool is_valid_pos(Position pos);

void Grid::set_start(Position pos) {
  assert(is_valid_pos(pos));
  assert(!s_obstacles.is_set(pos));
  s_start = pos;
}

void Grid::set_target(Position pos) {
  assert(is_valid_pos(pos));
  assert(!s_obstacles.is_set(pos));
  s_target = pos;
}

void Grid::set_obstacles(const BitGrid16x16& obstacles) {
  s_obstacles = obstacles;
}

Grid::Grid() {
  m_placeables.set(s_start);
  place(s_start);
}

Grid::Grid(const Grid& other)
    : m_placements{other.m_placements}
    , m_placeables{other.m_placeables} {};

Grid& Grid::operator=(Grid other) {
  std::swap(m_placements, other.m_placements);
  std::swap(m_placeables, other.m_placeables);
  return *this;
}

bool Grid::operator==(const Grid& other) const {
  return m_placements == other.m_placements;
}

std::ostream& operator<<(std::ostream& out, const Grid& grid) {
  for (int y{0}; y < Grid::MAX_Y; ++y) {
    for (int x{0}; x < Grid::MAX_X; ++x) {
      Position pos{x, y};

      if (pos == Grid::s_start) {
        out << 'S';
      } else if (grid.m_placements.is_set(pos)) {
        out << 'P';
      } else if (pos == Grid::s_target) {
        out << 'T';
      } else if (Grid::s_obstacles.is_set(pos)) {
        out << 'O';
      } else {
        out << '.';
      }

      out << ' ';
    }

    out << '\n';
  }

  return out;
}

std::size_t Grid::hash() const {
  return m_placements.hash();
}

void Grid::place(Position pos) {
  assert(is_valid_pos(pos));
  assert(m_placeables.is_set(pos));
  assert(!m_placements.is_set(pos));
  assert(!s_obstacles.is_set(pos));

  m_placements.set(pos);
  m_placeables.clear(pos);

  Position adj_positions[]{
      {pos.x, pos.y - 1}, {pos.x, pos.y + 1}, {pos.x - 1, pos.y}, {pos.x + 1, pos.y}
  };

  for (auto adj_pos : adj_positions) {
    if (is_valid_pos(adj_pos) && !m_placements.is_set(adj_pos) && !s_obstacles.is_set(adj_pos)) {
      m_placeables.set(adj_pos);
    }
  }
}

static bool is_valid_pos(Position pos) {
  return pos.x >= 0 && pos.x < Grid::MAX_X && pos.y >= 0 && pos.y < Grid::MAX_Y;
}
