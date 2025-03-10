#include "Grid.h"
#include "BitGrid16x16.h"
#include "Position.h"
#include <cassert>
#include <cstddef>
#include <ostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

static constexpr int TETROMINO_SIZE{4};

static bool is_valid_pos(Position pos);
static int heuristic(Position pos, Position target);

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

void Grid::preprocess_heuristic_values() {
  for (int y{0}; y < MAX_Y; ++y) {
    for (int x{0}; x < MAX_X; ++x) {
      s_heuristic_values[{x, y}] = heuristic({x, y}, s_target);
    }
  }
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

std::vector<Grid> Grid::successors() const {
  std::vector<Grid> successors{};
  std::unordered_set<BitGrid16x16, BitGrid16x16Hash> visited{};

  for (int y{0}; y < BitGrid16x16::MAX_Y; ++y) {
    for (int x{0}; x < BitGrid16x16::MAX_X; ++x) {
      if (m_placeables.is_set({x, y})) {
        successors.insert_range(successors.end(), successors_from({x, y}, visited));
      }
    }
  }

  return successors;
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

std::vector<Grid> Grid::successors_from(
    Position pos, std::unordered_set<BitGrid16x16, BitGrid16x16Hash>& visited
) const {
  struct Node {
    Grid grid{};
    std::unordered_set<Position, PositionHash> actions{};
    int depth{0};
  };

  std::vector<Grid> successors{};

  std::stack<Node> stack{};
  stack.emplace(*this, std::unordered_set<Position, PositionHash>{pos}, 0);

  while (!stack.empty()) {
    auto parent{std::move(stack.top())};
    stack.pop();

    if (parent.depth == TETROMINO_SIZE) {
      successors.push_back(std::move(parent.grid));
      continue;
    }

    for (auto action : parent.actions) {
      Node child{parent};
      child.grid.place(action);

      if (visited.contains(child.grid.m_placements)) {
        continue;
      } else {
        visited.insert(child.grid.m_placements);
      }

      child.actions.erase(action);
      ++child.depth;

      Position candidate_actions[]{
          {action.x, action.y - 1},
          {action.x, action.y + 1},
          {action.x - 1, action.y},
          {action.x + 1, action.y}
      };

      for (auto candidate_action : candidate_actions) {
        if (is_valid_pos(candidate_action) && !child.grid.m_placements.is_set(candidate_action)
            && !s_obstacles.is_set(candidate_action)) {
          child.actions.insert(candidate_action);
        }
      }

      stack.push(std::move(child));
    }
  }

  return successors;
}

static bool is_valid_pos(Position pos) {
  return pos.x >= 0 && pos.x < Grid::MAX_X && pos.y >= 0 && pos.y < Grid::MAX_Y;
}

static int heuristic(Position pos, Position target) {
  int manhattan_distance{std::abs(pos.x - target.x) + std::abs(pos.y - target.y)};
  return (manhattan_distance + (TETROMINO_SIZE - 1)) / TETROMINO_SIZE;
}
