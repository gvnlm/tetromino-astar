#ifndef NODE_H
#define NODE_H

#include "Grid.h"
#include <memory>
#include <ostream>
#include <vector>

class Node : public std::enable_shared_from_this<Node> {
public:
  Node() = default;

  Node(const Grid& grid, const std::shared_ptr<const Node>& parent)
      : m_grid{grid}
      , m_parent{parent} {};

  bool operator<(const Node& other) const;

  std::vector<std::shared_ptr<const Node>> successors() const;

  const Grid& grid() const;
  const std::shared_ptr<const Node>& parent() const;

private:
  Grid m_grid{};
  std::shared_ptr<const Node> m_parent{nullptr};
};

std::ostream& operator<<(std::ostream& out, const Node& node);

#endif
