#pragma once

#include "Piece.h"

class Rook : public Piece {
public:
  Rook(char piece, Square square) : Piece(piece, square) {}

  square_list getMoves() override;
  bool isValidMove(Square newSquare) override;
  bool hasRookMoved() { return hasMoved(); }
};
