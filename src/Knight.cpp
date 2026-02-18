#include "Knight.hpp"
#include <vector>

bool Knight::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;
    int moveX = abs(x - newSquare.x), moveY = abs(y - newSquare.y);

    if ((moveY == 2 && moveX == 1) || (moveX == 2 && moveY == 1))
        return true;

    return false;
}

square_list Knight::getMoves() {
    square_list moves;
    Square current = getSquare();

    // All 8 L-shaped moves
    int offsets[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};

    for (auto &offset : offsets) {
        int newX = current.x + offset[0];
        int newY = current.y + offset[1];
        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
            moves.push_back(Square{newX, newY, current.size});
        }
    }

    return moves;
}
