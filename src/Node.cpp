#include "../include/Node.h"
#include "../include/Grid.h"
#include <memory>
#include <vector>

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
