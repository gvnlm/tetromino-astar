#include "../include/Node.h"
#include "../include/Grid.h"
#include <algorithm>
#include <array>
#include <memory>
#include <vector>

Node::Node(const Grid& grid, const std::shared_ptr<const Node>& parent)
    : m_grid{grid}
    , m_parent{parent} {};

bool Node::operator<(const Node& other) const {
  return m_grid < other.m_grid;
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

std::ostream& operator<<(std::ostream& out, const Node& node) {
  // Unicode character symbols
  static constexpr char HOLLOW_SQUARE[]{"\u25A1"};
  static constexpr char SOLID_SQUARE[]{"\u25A0"};
  static constexpr char STAR[]{"\u2605"};
  static constexpr char CENTRE_DOT[]{"\u00B7"};

  // ANSI colour codes
  static constexpr char RED[]{"\033[31m"};
  static constexpr char YELLOW[]{"\033[33m"};
  static constexpr char RESET_COLOUR[]{"\033[0m"};

  std::array<Position, Grid::TETROMINO_SIZE> previous_move{};

  if (node.parent()) {
    previous_move = node.grid().difference(node.parent()->grid());
  }

  for (int y{0}; y < Grid::MAX_Y; ++y) {
    for (int x{0}; x < Grid::MAX_X; ++x) {
      Position pos{x, y};
      const auto& grid = node.grid();

      if (std::ranges::contains(previous_move, pos)) {
        out << RED << SOLID_SQUARE << RESET_COLOUR;
      } else if (pos == Grid::start() || grid.placements().is_set(pos)) {
        out << RED << HOLLOW_SQUARE << RESET_COLOUR;
      } else if (pos == Grid::target()) {
        out << YELLOW << STAR << RESET_COLOUR;
      } else if (Grid::obstacles().is_set(pos)) {
        out << SOLID_SQUARE;
      } else {
        out << CENTRE_DOT;
      }

      out << ' ';
    }

    out << '\n';
  }

  return out;
}
