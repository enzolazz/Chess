#pragma once

#include "Piece.hpp"

class Knight : public Piece {
 public:
    Knight(char piece, Square square) : Piece(piece, square) {}

    square_list getMoves() override;
    bool isValidMove(Square newSquare) override;
};
