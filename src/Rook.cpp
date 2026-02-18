#include "Rook.hpp"
#include <vector>

bool Rook::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;

    if (abs(x - newSquare.x) > 0 && (y - newSquare.y == 0) || abs(y - newSquare.y) > 0 && (x - newSquare.x == 0))
        return true;

    return false;
}

square_list Rook::getMoves() {
    square_list moves;
    Square current = getSquare();

    // Four directions: up, down, left, right
    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    for (auto &dir : directions) {
        for (int i = 1; i < 8; i++) {
            int newX = current.x + dir[0] * i;
            int newY = current.y + dir[1] * i;
            if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
                moves.push_back(Square{newX, newY, current.size});
            }
        }
    }

    return moves;
}
