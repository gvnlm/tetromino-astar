#include "astar.h"
#include "Grid.h"
#include "Node.h"
#include "Position.h"
#include <iostream>
#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

namespace {
struct Stats {
  int expanded{0};
  int generated{0};
  int revisited{0};

  friend std::ostream& operator<<(std::ostream& out, const Stats& stats) {
    out << "Nodes expanded: " << stats.expanded << '\n';
    out << "Nodes generated: " << stats.generated << '\n';
    out << "Nodes revisited: " << stats.revisited << '\n';

    return out;
  }
};

auto make_priority_queue() {
  auto node_ptr_cmp
      = [](const std::shared_ptr<const Node>& a, const std::shared_ptr<const Node>& b) {
          return *a < *b;
        };

  return std::priority_queue<
      std::shared_ptr<const Node>,
      std::vector<std::shared_ptr<const Node>>,
      decltype(node_ptr_cmp)>{node_ptr_cmp};
}
}

void astar(
    Position start, Position target, const std::vector<Position>& obstacles, bool visualise
) {
  Grid::set_start(start);
  Grid::set_target(target);
  Grid::set_obstacles(obstacles);
  Grid::preprocess_heuristic_values();

  auto root{std::make_shared<const Node>()};
  std::cout << root->grid() << '\n';

  if (Grid::is_target_enclosed()) {
    std::cout << "The target is enclosed - no solution exists.\n";
    return;
  }

  auto priority_queue{make_priority_queue()};
  std::unordered_set<Grid, GridHash> visited{};
  Stats stats{};

  if (!visualise) {
    std::cout << "Searching for an optimal solution...\n";
  }

  priority_queue.push(std::move(root));

  while (!priority_queue.empty()) {
    auto best{priority_queue.top()};
    priority_queue.pop();

    if (visualise) {
      std::cout << "\033[" << Grid::MAX_Y + 1 << 'A'; // Move cursor up Grid::MAX_Y + 1 times
      std::cout << "\033[J";                          // Clear screen starting from cursor
      std::cout << best->grid() << '\n';
    }

    if (best->grid().is_target_reached()) {
      std::cout << "Found an optimal solution!\n\n";

      if (!visualise) {
        std::cout << best->grid() << '\n';
      }

      std::cout << stats << '\n';
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
