#pragma once

#include "Piece.h"

class Bishop : public Piece {
public:
  Bishop(char piece, Square square) : Piece(piece, square) {}

  square_list getMoves() override;
  bool isValidMove(Square newSquare) override;
};
