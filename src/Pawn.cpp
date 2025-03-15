#include "Pawn.h"

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
  square_list s;

  s.push_back(Square{3, 3, getSquare().size});
  s.push_back(Square{3, 4, getSquare().size});
  s.push_back(Square{3, 5, getSquare().size});

  return s;
}
