#include "../include/Grid.h"
#include "../include/BitGrid.h"
#include "../include/Position.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ostream>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

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

void Grid::set_obstacles(const BitGrid<MAX_X, MAX_Y>& obstacles) {
  s_obstacles = obstacles;
}

void Grid::preprocess_heuristic_values() {
  struct Node {
    Position pos;
    int cost;

    bool operator<(const Node& other) const {
      return cost > other.cost;
    }
  };

  std::priority_queue<Node> unvisited{};
  std::unordered_set<Position, PositionHash> visited{};

  unvisited.emplace(s_target, 0);
  s_heuristic_values[s_target] = 0;

  while (!unvisited.empty()) {
    auto curr{unvisited.top()};
    unvisited.pop();

    visited.insert(curr.pos);

    Position adj_positions[]{
        {curr.pos.x, curr.pos.y - 1},
        {curr.pos.x, curr.pos.y + 1},
        {curr.pos.x - 1, curr.pos.y},
        {curr.pos.x + 1, curr.pos.y}
    };

    for (const auto& adj_pos : adj_positions) {
      if (visited.contains(adj_pos) || !is_valid_pos(adj_pos) || s_obstacles.is_set(adj_pos)) {
        continue;
      }

      int adj_cost{curr.cost + 1};

      if (!s_heuristic_values.contains(adj_pos) || adj_cost < s_heuristic_values[adj_pos]) {
        s_heuristic_values[adj_pos] = adj_cost;
        unvisited.emplace(adj_pos, adj_cost);
      }
    }
  }

  for (auto& [_, heuristic_value] : s_heuristic_values) {
    heuristic_value = (heuristic_value + (TETROMINO_SIZE - 1)) / TETROMINO_SIZE;
  }
}

bool Grid::is_target_enclosed() {
  assert(
      !s_heuristic_values.empty()
      && "Grid::preprocess_heuristic_values() must be called before calling "
         "Grid::is_target_enclosed()"
  );

  return !s_heuristic_values.contains(s_start);
}

Grid::Grid() {
  assert(
      !s_heuristic_values.empty()
      && "Grid::preprocess_heuristic_values() must be called before initialising instances of Grid"
  );

  if (s_heuristic_values.contains(s_start)) {
    m_h = s_heuristic_values[s_start];
    m_placeables.set(s_start);
    place(s_start);
  }
}

Grid::Grid(const Grid& other)
    : m_placements{other.m_placements}
    , m_placeables{other.m_placeables}
    , m_g{other.m_g}
    , m_h{other.m_h} {};

Grid& Grid::operator=(Grid other) {
  std::swap(m_placements, other.m_placements);
  std::swap(m_placeables, other.m_placeables);
  std::swap(m_g, other.m_g);
  std::swap(m_h, other.m_h);
  return *this;
}

bool Grid::operator==(const Grid& other) const {
  return m_placements == other.m_placements;
}

bool Grid::operator<(const Grid& other) const {
  return m_g + m_h > other.m_g + other.m_h;
}

std::ostream& operator<<(std::ostream& out, const Grid& grid) {
  for (int y{0}; y < Grid::MAX_Y; ++y) {
    for (int x{0}; x < Grid::MAX_X; ++x) {
      Position pos{x, y};

      if (pos == Grid::s_start || grid.m_placements.is_set(pos)) {
        out << "\u25A1";
      } else if (pos == Grid::s_target) {
        out << "\u2605";
      } else if (Grid::s_obstacles.is_set(pos)) {
        out << "\u25A0";
      } else {
        out << "\u00B7";
      }

      out << ' ';
    }

    out << '\n';
  }

  return out;
}

std::vector<Grid> Grid::successors() const {
  std::vector<Grid> successors{};
  std::unordered_set<BitGrid<MAX_X, MAX_Y>, BitGridHash<MAX_X, MAX_Y>> visited{};

  for (int y{0}; y < BitGrid<MAX_X, MAX_Y>::MAX_Y; ++y) {
    for (int x{0}; x < BitGrid<MAX_X, MAX_Y>::MAX_X; ++x) {
      if (m_placeables.is_set({x, y})) {
        successors.insert_range(successors.end(), successors_from({x, y}, visited));
      }
    }
  }

  return successors;
}

bool Grid::is_target_reached() const {
  return m_h == 0;
}

std::array<Position, Grid::TETROMINO_SIZE> Grid::difference(const Grid& other) const {
  std::array<Position, TETROMINO_SIZE> difference{};
  auto difference_it{difference.begin()};

  for (int x{0}; x < MAX_X; ++x) {
    for (int y{0}; y < MAX_Y; ++y) {
      if (m_placements.is_set({x, y}) != other.m_placements.is_set({x, y})) {
        assert(difference_it != difference.end());

        *difference_it = {x, y};
        ++difference_it;
      }
    }
  }

  return difference;
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
  m_h = std::min(s_heuristic_values[pos], m_h);

  Position adj_positions[]{
      {pos.x, pos.y - 1}, {pos.x, pos.y + 1}, {pos.x - 1, pos.y}, {pos.x + 1, pos.y}
  };

  for (const auto& adj_pos : adj_positions) {
    if (is_valid_pos(adj_pos) && !m_placements.is_set(adj_pos) && !s_obstacles.is_set(adj_pos)) {
      m_placeables.set(adj_pos);
    }
  }
}

std::vector<Grid> Grid::successors_from(
    Position pos, std::unordered_set<BitGrid<MAX_X, MAX_Y>, BitGridHash<MAX_X, MAX_Y>>& visited
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
      ++parent.grid.m_g;
      successors.push_back(std::move(parent.grid));
      continue;
    }

    for (const auto& action : parent.actions) {
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

      for (const auto& candidate_action : candidate_actions) {
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
