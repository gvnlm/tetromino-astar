#include "../include/Node.h"
#include "../include/Grid.h"
#include <algorithm>
#include <array>
#include <memory>
#include <vector>

bool Node::operator<(const Node& other) const {
  return m_grid < other.m_grid;
}

std::ostream& operator<<(std::ostream& out, const Node& node) {
  std::array<Position, Grid::TETROMINO_SIZE> previous_move{};

  if (node.parent()) {
    previous_move = node.grid().difference(node.parent()->grid());
  }

  for (int y{0}; y < Grid::MAX_Y; ++y) {
    for (int x{0}; x < Grid::MAX_X; ++x) {
      Position pos{x, y};
      auto grid = node.grid();

      if (std::ranges::contains(previous_move, pos)) {
        out << "\033[31m" << "\u25A0" << "\033[0m";
      } else if (pos == Grid::start() || grid.placements().is_set(pos)) {
        out << "\033[31m" << "\u25A1" << "\033[0m";
      } else if (pos == Grid::target()) {
        out << "\033[33m" << "\u2605" << "\033[0m";
      } else if (Grid::obstacles().is_set(pos)) {
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

std::vector<std::shared_ptr<const Node>> Node::successors() const {
  std::vector<std::shared_ptr<const Node>> successors{};

  for (const auto& grid : m_grid.successors()) {
    auto successor{std::make_shared<const Node>(grid, shared_from_this())};
    successors.push_back(std::move(successor));
  }

  return successors;
}

const Grid& Node::grid() const {
  return m_grid;
}
const std::shared_ptr<const Node>& Node::parent() const {
  return m_parent;
}
