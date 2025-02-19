#pragma once

#ifndef HEADERS_PIECES_BISHOP_H_
#define HEADERS_PIECES_BISHOP_H_

#include "Piece.h"

class Bishop: public Piece {
public:
    Bishop(char piece, Square square) : Piece(piece, square) {}

    square_list getMoves() override;
    bool isValidMove(Square newSquare) override;
};

#endif  // HEADERS_PIECES_BISHOP_H_
