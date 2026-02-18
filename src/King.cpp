#include "King.hpp"
#include <vector>

bool King::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;

    if (abs(x - newSquare.x) <= 1 && abs(y - newSquare.y) <= 1)
        return true;

    if ((y - newSquare.y == 0) && ((x - newSquare.x >= 2) || (x - newSquare.x <= -2)))
        return !hasMoved();

    return false;
}

square_list King::getMoves() {
    square_list moves;
    Square current = getSquare();

    // All 8 directions + castling squares
    int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    for (auto &dir : directions) {
        int newX = current.x + dir[0];
        int newY = current.y + dir[1];
        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
            moves.push_back(Square{newX, newY, current.size});
        }
    }

    // Castling squares (will be validated by Game)
    if (!hasMoved()) {
        moves.push_back(Square{current.x - 2, current.y, current.size}); // Queen-side
        moves.push_back(Square{current.x + 2, current.y, current.size}); // King-side
    }

    return moves;
}
