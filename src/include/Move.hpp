#pragma once

#include "Piece.hpp"
#include <cctype>
#include <cstdlib>
#include <memory>

struct Move {
    std::shared_ptr<Piece> piece;     // Piece that moved
    Square from;                       // Origin square
    std::shared_ptr<Piece> captured;  // Captured piece (nullptr if none)
    Square to;                         // Destination square
    bool orientation;                  // Board orientation at the time

    Move(std::shared_ptr<Piece> p, Square f, std::shared_ptr<Piece> c, Square t, bool o)
        : piece(p), from(f), captured(c), to(t), orientation(o) {}

    bool isCapture() const { return captured != nullptr; }

    bool isCastle() const {
        return piece && std::tolower(piece->getType()) == 'k' && abs(from.x - to.x) == 2;
    }
};
