#include "Rook.h"
#include <vector>

bool Rook::isValidMove(Square newSquare) {
  int x = getSquare().x, y = getSquare().y;

  if (abs(x - newSquare.x) > 0 && (y - newSquare.y == 0) ||
      abs(y - newSquare.y) > 0 && (x - newSquare.x == 0))
    return true;

  return false;
}

square_list Rook::getMoves() { return std::vector<Square>(); }
