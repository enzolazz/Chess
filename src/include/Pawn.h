#pragma once

#include "Piece.h"

class Pawn : public Piece {
private:
  bool isInInitialRow(bool boardOrientation);

public:
  Pawn(char piece, Square square) : Piece(piece, square) {}

  square_list getMoves() override;
  bool isValidMove(Square newSquare, bool boardOrientation);
  bool isValidMove(Square newSquare) override;
};
