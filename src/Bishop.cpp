#include "Bishop.hpp"
#include <vector>

bool Bishop::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;

    if (abs(x - newSquare.x) == abs(y - newSquare.y))
        return true;

    return false;
}

square_list Bishop::getMoves() { return std::vector<Square>(); }
