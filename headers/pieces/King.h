#pragma once

#ifndef HEADERS_PIECES_KING_H_
#define HEADERS_PIECES_KING_H_

#include "Piece.h"

class King: public Piece {
public:
    King(char piece, Square square) : Piece(piece, square) {}

    square_list getMoves() override;
    bool isValidMove(Square newSquare) override;
};

#endif  // HEADERS_PIECES_KING_H_
