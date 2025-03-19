#ifndef NODE_H
#define NODE_H

#include "Grid.h"
#include <memory>
#include <ostream>
#include <vector>

/**
 * Represents a search node for A* search with tetromino pieces.
 *
 * Encapsulates a grid state, and the node's parent to allow for backtracking.
 * Inherits from `std::enable_shared_from_this` to allow for the creation of successor nodes that
 * references their parent. Because of this, nodes must always be created, managed, and referenced
 * via shared pointers.
 */
class Node : public std::enable_shared_from_this<Node> {
public:
  Node() = default;
  Node(const Grid& grid, const std::shared_ptr<const Node>& parent);

  bool operator<(const Node& other) const;

  /**
   * Returns a vector containing all successor nodes.
   *
   * A successor node encapsulates a successor grid and a reference to its parent (i.e., the calling
   * node).
   * The successor grid is the result of validly placing a tetromino on `m_grid` (see
   * `successors()` in `Grid` for more detail).
   */
  std::vector<std::shared_ptr<const Node>> successors() const;

  const Grid& grid() const;
  const std::shared_ptr<const Node>& parent() const;

private:
  Grid m_grid{};
  std::shared_ptr<const Node> m_parent{nullptr};
};

std::ostream& operator<<(std::ostream& out, const Node& node);

#endif
