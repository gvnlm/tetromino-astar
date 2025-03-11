#ifndef ASTAR_H
#define ASTAR_H

#include "Position.h"
#include <vector>

void astar(
    Position start, Position target, const std::vector<Position>& obstacles, bool visualise = false
);

#endif
