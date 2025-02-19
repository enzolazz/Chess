#include <vector>
#include "../../headers/pieces/King.h"

bool King::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;

    if (abs(x - newSquare.x) <= 1 && abs(y - newSquare.y) <= 1)
        return true;

    if ((y - newSquare.y == 0) && ((x - newSquare.x >= 2) || (x - newSquare.x <= -2)))
        return !hasMoved();

    return false;
}

square_list King::getMoves() {
    return std::vector<Square>();
}
