#ifndef ASTAR_H
#define ASTAR_H

#include "Position.h"
#include <string>
#include <vector>

struct AstarParams {
  Position start;
  Position target;
  std::vector<Position> obstacles;
};

bool read_astar_params(const std::string& filename, AstarParams& params);
void astar(
    Position start, Position target, const std::vector<Position>& obstacles, bool visualise = false
);

#endif
