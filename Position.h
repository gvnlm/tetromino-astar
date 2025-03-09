#ifndef POSITION_H
#define POSITION_H

struct Position {
  int x{0};
  int y{0};

  bool operator==(Position other) const {
    return x == other.x && y == other.y;
  }
};

#endif
