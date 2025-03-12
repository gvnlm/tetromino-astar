#include "astar.h"
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Error: Missing input file.\n";
    return 0;
  }

  AstarParams params{};

  if (!read_astar_params(argv[1], params)) {
    std::cout
        << "Error: Input file does not follow the expected format as described in README.md.\n";
    return 0;
  }

  astar(params.start, params.target, params.obstacles, true);

  return 0;
}
