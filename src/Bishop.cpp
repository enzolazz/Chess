#include "Bishop.hpp"
#include <vector>

bool Bishop::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;

    if (abs(x - newSquare.x) == abs(y - newSquare.y))
        return true;

    return false;
}

square_list Bishop::getMoves() {
    square_list moves;
    Square current = getSquare();

    // Four diagonal directions
    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

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
