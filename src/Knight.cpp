#include "Knight.hpp"
#include <vector>

bool Knight::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;
    int moveX = abs(x - newSquare.x), moveY = abs(y - newSquare.y);

    if ((moveY == 2 && moveX == 1) || (moveX == 2 && moveY == 1))
        return true;

    return false;
}

square_list Knight::getMoves() { return std::vector<Square>(); }
