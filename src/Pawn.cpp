#include "Pawn.hpp"

bool Pawn::isInInitialRow(bool boardOrientation) {
    int row = getSquare().y + 1;
    int initial;

    if (isWhite()) {
        if (boardOrientation)
            initial = 7;
        else
            initial = 2;
    } else {
        if (boardOrientation)
            initial = 2;
        else
            initial = 7;
    }

    return initial == row;
}

bool Pawn::isValidMove(Square newSquare) { return false; }

bool Pawn::isValidMove(Square newSquare, bool boardOrientation) {
    bool initial = isInInitialRow(boardOrientation);

    int walk = (initial ? 2 : 1);
    int moveX = getSquare().x - newSquare.x, moveY = getSquare().y - newSquare.y;

    if (!isWhite())
        moveY *= -1;
    if (!boardOrientation)
        moveY *= -1;

    if ((moveY > 0 && moveY <= walk) && (moveX >= -1 && moveX <= 1))
        return true;

    return false;
}

square_list Pawn::getMoves() {
    square_list moves;
    Square current = getSquare();

    // Direction depends on color (white moves up, black moves down)
    // This is a simplified version - actual validation happens in Game
    int direction = isWhite() ? -1 : 1;

    // Forward move
    int newY = current.y + direction;
    if (newY >= 0 && newY < 8) {
        moves.push_back(Square{current.x, newY, current.size});

        // Double move from starting position
        int startRow = isWhite() ? 6 : 1;
        if (current.y == startRow) {
            moves.push_back(Square{current.x, current.y + 2 * direction, current.size});
        }
    }

    // Diagonal captures
    if (current.x > 0 && newY >= 0 && newY < 8) {
        moves.push_back(Square{current.x - 1, newY, current.size});
    }
    if (current.x < 7 && newY >= 0 && newY < 8) {
        moves.push_back(Square{current.x + 1, newY, current.size});
    }

    return moves;
}
