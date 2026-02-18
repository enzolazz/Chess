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

square_list Queen::getMoves() {
    square_list moves;
    Square current = getSquare();

    // All 8 directions (rook + bishop)
    int directions[8][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

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
