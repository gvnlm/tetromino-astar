#include "../include/astar.h"
#include "../include/Grid.h"
#include "../include/Node.h"
#include "../include/Position.h"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

namespace {
// Node stats for A* search
struct Stats {
  int expanded{0};
  int generated{0};
  int revisited{0};

  friend std::ostream& operator<<(std::ostream& out, const Stats& stats) {
    out << "In total,\n";
    out << stats.expanded << " nodes were expanded,\n";
    out << stats.generated << " nodes were generated, and\n";
    out << stats.revisited << " nodes were revisited.\n";

    return out;
  }
};

auto make_priority_queue() { // For A* search
  auto node_ptr_cmp
      = [](const std::shared_ptr<const Node>& a, const std::shared_ptr<const Node>& b) {
          return *a < *b;
        };

  return std::priority_queue<
      std::shared_ptr<const Node>,
      std::vector<std::shared_ptr<const Node>>,
      decltype(node_ptr_cmp)>{node_ptr_cmp};
}

void clear_grid_display() {
  // Move cursor up Grid::MAX_Y + 1 times (assumes the most recent output is a grid followed by a
  // newline)
  std::cout << "\033[" << Grid::MAX_Y + 1 << 'A';
  // Clear screen beginning from cursor
  std::cout << "\033[J";
}

void display_path_interactive(const std::shared_ptr<const Node>& node) {
  // Number of newlines after the previously printed grid
  static constexpr int NUM_OF_NEWLINES_AFTER_GRID{5};

  // Represents path to `node`, where `path[i + 1]` is `path[i]`'s parent
  std::vector<std::shared_ptr<const Node>> path{};

  std::size_t path_index{0};
  std::string input{};

  for (auto curr{node}; curr; curr = curr->parent()) {
    path.emplace_back(curr);
  }

  std::cout << *path[path_index] << '\n';
  std::cout << "Move: " << path.size() - path_index - 1 << '\n';
  std::cout << "Press 'n' then Enter to see the next move.\n";
  std::cout << "Press 'b' then Enter to see the previous move.\n";
  std::cout << "Press Enter to exit.\n";

  while (std::getline(std::cin, input) && !input.empty()) {
    if (input == "b") {
      path_index = (path_index == path.size() - 1) ? 0 : path_index + 1;
    } else if (input == "n") {
      path_index = (path_index == 0) ? path.size() - 1 : path_index - 1;
    } else {
      return;
    }

    // Move cursor up Grid::MAX_Y + 1 + NUM_OF_NEWLINES_AFTER_GRID times
    std::cout << "\033[" << Grid::MAX_Y + 1 + NUM_OF_NEWLINES_AFTER_GRID << 'A';
    // Clear screen starting from cursor
    std::cout << "\033[J";

    std::cout << *path[path_index] << '\n';
    std::cout << "Move: " << path.size() - path_index - 1 << '\n';
    std::cout << "Press 'n' then Enter to see the next move.\n";
    std::cout << "Press 'b' then Enter to see the previous move.\n";
    std::cout << "Press Enter to exit.\n";
  }
}
}

bool read_astar_params(const std::string& filename, AstarParams& params) {
  std::ifstream file(filename);

  if (!file) {
    return false;
  }

  bool start_found{false};
  bool target_found{false};
  std::string line{};

  for (int y{0}; y < Grid::MAX_Y; ++y) {
    std::getline(file, line);

    for (int x{0}; x < Grid::MAX_X; ++x) {
      char ch{line.data()[x]};

      if (ch == 's' || ch == 'S') {
        if (start_found) {
          return false;
        }

        params.start = {x, y};
        start_found = true;
        continue;
      }

      if (ch == 't' || ch == 'T') {
        if (target_found) {
          return false;
        }

        params.target = {x, y};
        target_found = true;
        continue;
      }

      if (ch == 'o' || ch == 'O') {
        params.obstacles.emplace_back(x, y);
        continue;
      }

      if (ch != '.') {
        return false;
      }
    }
  }

  return start_found && target_found;
}

void astar(
    Position start, Position target, const std::vector<Position>& obstacles, bool visualise
) {
  Grid::set_start(start);
  Grid::set_target(target);
  Grid::set_obstacles(obstacles);
  Grid::preprocess_heuristic_values();

  auto root{std::make_shared<const Node>()};

  if (Grid::is_target_enclosed()) {
    std::cout << "The target is enclosed - no solution exists.\n";
    return;
  }

  std::unordered_set<Grid, GridHash> visited{};
  Stats stats{};

  auto priority_queue{make_priority_queue()};
  priority_queue.push(std::move(root));

  std::cout << "Searching for an optimal solution...\n\n";

  if (visualise) {
    std::cout << *priority_queue.top() << '\n';
  }

  while (!priority_queue.empty()) {
    auto best{priority_queue.top()};
    priority_queue.pop();

    if (visualise) {
      clear_grid_display();
      std::cout << *best << '\n';
    }

    if (best->grid().is_target_reached()) {
      if (visualise) {
        clear_grid_display();
      }

      std::cout << "Found an optimal solution!\n\n";
      std::cout << stats << '\n';
      display_path_interactive(best);

      return;
    }

    for (const auto& successor : best->successors()) {
      ++stats.generated;

      if (!visited.contains(successor->grid())) {
        visited.insert(successor->grid());
        priority_queue.push(std::move(successor));
      } else {
        ++stats.revisited;
      }
    }

    ++stats.expanded;
  }

  std::cout << "An optimal solution could not be found.\n";
}
