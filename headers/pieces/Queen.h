#pragma once

#ifndef HEADERS_PIECES_QUEEN_H_
#define HEADERS_PIECES_QUEEN_H_

#include "Piece.h"

class Queen: public Piece {
public:
    Queen(char piece, Square square) : Piece(piece, square) {}

    square_list getMoves() override;
    bool isValidMove(Square newSquare) override;
};

#endif  // HEADERS_PIECES_QUEEN_H_
