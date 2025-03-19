#ifndef ASTAR_H
#define ASTAR_H

#include "Position.h"
#include <string>
#include <vector>

/**
 * Stores the required parameters of `astar()`.
 */
struct AstarParams {
  Position start;
  Position target;
  std::vector<Position> obstacles;
};

/**
 * Reads the required parameters for `astar()` from file `filename` and stores them in `params`.
 * Returns `true` if successful, otherwise `false`.
 *
 * The file should be a .txt file, containing a string representation of the initial grid state,
 * where 's' represents the start position, 't' represents the target position, 'o' represents
 * obstacle positions, and '.' represents empty positions. Here is an example of a valid file:
 * s.......................
 * ........................
 * ........................
 * ........................
 * ........................
 * ........................
 * .............o..........
 * ............o...........
 * ...........o............
 * ..........o.............
 * .........o..............
 * ........................
 * ........................
 * ........................
 * ........................
 * .......................t
 */
bool read_astar_params(const std::string& filename, AstarParams& params);

/**
 * Searches for an optimal path from the start position to the target position, avoiding obstacles
 * positions, where moves are limited to placing tetrominos. Once the optimal path is found, an
 * interactive console display allows for move-by-move visualisation of the optimal path found.
 *
 * If `visualise` is `true`, each grid expanded during the search is displayed to the console.
 */
void astar(
    Position start, Position target, const std::vector<Position>& obstacles, bool visualise = false
);

#endif
