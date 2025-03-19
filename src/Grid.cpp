#include "../include/Grid.h"
#include "../include/BitGrid.h"
#include "../include/Position.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {
bool is_valid_pos(Position pos) {
  return pos.x >= 0 && pos.x < Grid::MAX_X && pos.y >= 0 && pos.y < Grid::MAX_Y;
}
}

void Grid::set_start(Position pos) {
  assert(is_valid_pos(pos));
  s_start = pos;
}

void Grid::set_target(Position pos) {
  assert(is_valid_pos(pos));
  s_target = pos;
}

void Grid::set_obstacles(const BitGrid<MAX_X, MAX_Y>& obstacles) {
  s_obstacles = obstacles;
}

void Grid::preprocess_heuristic_values() {
  /**
   * Use Dijkstra's algorithm to calculate the optimal cost from the target position to every
   * non-obstacle position.
   */

  struct Node {
    Position pos;
    int cost;

    bool operator<(Node other) const {
      return cost > other.cost;
    }
  };

  std::priority_queue<Node> unvisited{};
  std::unordered_set<Position, PositionHash> visited_positions{};

  unvisited.emplace(s_target, 0);
  s_heuristic_values[s_target] = 0;

  while (!unvisited.empty()) {
    auto curr{unvisited.top()};
    unvisited.pop();

    // Finalise optimal cost from target position to `curr.pos`
    visited_positions.insert(curr.pos);

    Position adj_positions[]{
        {curr.pos.x, curr.pos.y - 1},
        {curr.pos.x, curr.pos.y + 1},
        {curr.pos.x - 1, curr.pos.y},
        {curr.pos.x + 1, curr.pos.y}
    };

    // Update neighbours' costs
    for (auto adj_pos : adj_positions) {
      // Ignore visited, invalid, or obstacle positions
      if (visited_positions.contains(adj_pos) || !is_valid_pos(adj_pos)
          || s_obstacles.is_set(adj_pos)) {
        continue;
      }

      int adj_cost{curr.cost + 1};

      if (!s_heuristic_values.contains(adj_pos) || adj_cost < s_heuristic_values[adj_pos]) {
        s_heuristic_values[adj_pos] = adj_cost;
        unvisited.emplace(adj_pos, adj_cost);
      }
    }
  }

  // Measure cost in terms of tetromino moves instead of single cell moves
  for (auto& [_, heuristic_value] : s_heuristic_values) {
    heuristic_value = (heuristic_value + (TETROMINO_SIZE - 1)) / TETROMINO_SIZE;
  }
}

bool Grid::is_target_enclosed() {
  /**
   * Assuming `preprocess_heuristic_values()` has been called (which implements Dijkstra's
   * algorithm), `s_heuristic_values.contains(s_start)` is `true` if and only if a path from the
   * target position to the start position exists.
   *
   * Dependent on `s_heuristic_values` containing only positions reachable from the target position.
   */

  assert(
      !s_heuristic_values.empty()
      && "Ensure Grid::preprocess_heuristic_values() has been called before calling "
         "Grid::is_target_enclosed()"
  );

  return !s_heuristic_values.contains(s_start);
}

Position Grid::start() {
  return s_start;
}

Position Grid::target() {
  return s_target;
}

const BitGrid<Grid::MAX_X, Grid::MAX_Y>& Grid::obstacles() {
  return s_obstacles;
}

Grid::Grid() {
  assert(
      !s_heuristic_values.empty()
      && "Ensure `Grid::preprocess_heuristic_values()` has been called before initialising "
         "instances of Grid"
  );

  // Do not call `s_heuristic_values[s_start]` if target is enclosed, otherwise
  // future calls to `is_target_enclosed()` will break (since it is dependent on
  // `s_heuristic_values` containing only positions reachable from the target position)
  if (!is_target_enclosed()) {
    m_h = s_heuristic_values[s_start];
    m_placeables.set(s_start);
    place(s_start);
  }
}

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
  // Grids with lower cost are considered greater
  return (m_g + m_h) > (other.m_g + other.m_h);
}

std::size_t Grid::hash() const {
  return m_placements.hash();
}

std::vector<Grid> Grid::successors() const {
  std::vector<Grid> successors{};
  std::unordered_set<BitGrid<MAX_X, MAX_Y>, BitGridHash<MAX_X, MAX_Y>> visited{};

  // For each position (x, y) adjacent to at least 1 position where a piece has been placed...
  for (int y{0}; y < BitGrid<MAX_X, MAX_Y>::MAX_Y; ++y) {
    for (int x{0}; x < BitGrid<MAX_X, MAX_Y>::MAX_X; ++x) {
      if (m_placeables.is_set({x, y})) {
        // ...add all successor grids that result from placing a tetromino on the calling grid that
        // overlaps (x, y)
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

const BitGrid<Grid::MAX_X, Grid::MAX_Y>& Grid::placements() const {
  return m_placements;
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

  for (auto adj_pos : adj_positions) {
    if (is_valid_pos(adj_pos) && !m_placements.is_set(adj_pos) && !s_obstacles.is_set(adj_pos)) {
      m_placeables.set(adj_pos);
    }
  }
}

std::vector<Grid> Grid::successors_from(
    Position pos, std::unordered_set<BitGrid<MAX_X, MAX_Y>, BitGridHash<MAX_X, MAX_Y>>& visited
) const {
  /* Implemented using iterative depth-limited search, where depth is limited to 4. The actions a
   * node can take involve placing a piece adjacent to an already placed piece (in this search, not
   * the entire grid). Since initially there are no previously placed pieces, position `pos` is used
   * as the initial action, thus ensuring `pos` is overlapped. Nodes at depth 4 (i.e., have taken 4
   * actions) contain the successor grids.
   */

  struct Node {
    Grid grid{};
    std::unordered_set<Position, PositionHash> actions{};
    int depth{0};
  };

  std::vector<Grid> successors{};

  std::stack<Node> stack{}; // For depth-limited search
  stack.emplace(*this, std::unordered_set<Position, PositionHash>{pos}, 0);

  while (!stack.empty()) {
    auto parent{std::move(stack.top())};
    stack.pop();

    if (parent.depth == TETROMINO_SIZE) {
      ++parent.grid.m_g;
      successors.push_back(std::move(parent.grid));
      continue;
    }

    // Generate children then explore them
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
