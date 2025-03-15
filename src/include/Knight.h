#pragma once

#include "Piece.h"

class Knight : public Piece {
public:
  Knight(char piece, Square square) : Piece(piece, square) {}

  square_list getMoves() override;
  bool isValidMove(Square newSquare) override;
};
