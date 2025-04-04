#pragma once

#include "Piece.hpp"

class Queen : public Piece {
 public:
    Queen(char piece, Square square) : Piece(piece, square) {}

    square_list getMoves() override;
    bool isValidMove(Square newSquare) override;
};
