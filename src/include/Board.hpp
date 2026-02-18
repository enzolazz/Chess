#pragma once

#include "Piece.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

#define WHITECOLOR sf::Color(240, 217, 181)
#define BLACKCOLOR sf::Color(181, 136, 99)
#define MOVECOLOR sf::Color(237, 234, 83, 200)

constexpr float BOARD_MARGIN = 0.f;

class Board {
 private:
    float squareSize;
    bool orientation;
    std::vector<std::vector<std::shared_ptr<Piece>>> pieces;

    sf::Color findColor(int i, int j);
    void initialBoard(std::string setup);
    void setPiece(char pieceType, Square square);
    void setPiece(std::shared_ptr<Piece> piece, Square square);
    void deletePiece(int x, int y);

#define CHECKCOLOR sf::Color(235, 97, 80, 200)

 public:
    sf::RectangleShape board[8][8];
    std::array<std::unique_ptr<sf::RectangleShape>, 2> moveSquare;
    std::unique_ptr<sf::RectangleShape> checkSquare;

    // Store logical positions for move highlight (to repaint after flip)
    Square lastMoveFrom{-1, -1, 0};
    Square lastMoveTo{-1, -1, 0};

    Board(float squareSize, std::string piecesSetup);

    std::shared_ptr<Piece> &getPiece(int x, int y);
    bool movePiece(std::shared_ptr<Piece> piece, Square square);
    void invertPosition();
    void setColor(sf::Color newColor);
    std::shared_ptr<Piece> createPiece(char piece, Square square);
    void paintMove(Square old, Square moved);
    void resetColors();
    bool isPainted();
    bool getOrientation();

    Square findKing(bool isWhite);
    bool isSquareAttacked(Square target, bool byWhite);
    bool isKingInCheck(bool isWhite);
    void highlightCheck(bool isWhite, bool highlight);
};
