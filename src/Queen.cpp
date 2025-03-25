#include "Queen.hpp"
#include <vector>

bool Queen::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;

    if (abs(x - newSquare.x) == abs(y - newSquare.y))
        return true;

    if (abs(x - newSquare.x) > 0 && (y - newSquare.y == 0) || abs(y - newSquare.y) > 0 && (x - newSquare.x == 0))
        return true;

    return false;
}

square_list Queen::getMoves() { return std::vector<Square>(); }
