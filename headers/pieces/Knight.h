#pragma once

#ifndef HEADERS_PIECES_KNIGHT_H_
#define HEADERS_PIECES_KNIGHT_H_

#include "Piece.h"

class Knight: public Piece {
public:
    Knight(char piece, Square square) : Piece(piece, square) {}

    square_list getMoves() override;
    bool isValidMove(Square newSquare) override;
};

#endif  // HEADERS_PIECES_KNIGHT_H_
