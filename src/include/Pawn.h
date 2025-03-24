#pragma once

#include "Piece.h"

class Pawn : public Piece {
 private:
    bool enPassant = false;
    bool isInInitialRow(bool boardOrientation);

 public:
    Pawn(char piece, Square square) : Piece(piece, square) {}

    square_list getMoves() override;
    bool isValidMove(Square newSquare, bool boardOrientation);
    bool isValidMove(Square newSquare) override;
    void allowEnPassant(bool value);
    bool getEnPassant();
};
