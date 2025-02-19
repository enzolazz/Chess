#pragma once

#ifndef HEADERS_PIECES_ROOK_H_
#define HEADERS_PIECES_ROOK_H_

#include "Piece.h"

class Rook : public Piece {
public:
    Rook(char piece, Square square) : Piece(piece, square) {}

    square_list getMoves() override;
    bool isValidMove(Square newSquare) override;
    bool hasRookMoved() {
        return hasMoved();
    }
};

#endif  // HEADERS_PIECES_ROOK_H_
