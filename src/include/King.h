#pragma once

#include "Piece.h"

class King : public Piece {
public:
  King(char piece, Square square) : Piece(piece, square) {}

  square_list getMoves() override;
  bool isValidMove(Square newSquare) override;
};
