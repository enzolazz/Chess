#pragma once

#include "Piece.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

class Board {
 private:
    sf::Color whiteColor = sf::Color(240, 217, 181); // Branco clássico
    sf::Color blackColor = sf::Color(181, 136, 99);  // Marrom clássico
    sf::Color moveColor = sf::Color(237, 234, 83);   // Marrom clássico

    float squareSize;
    bool orientation;
    std::vector<std::vector<std::shared_ptr<Piece>>> pieces;

    sf::Color findColor(int i, int j);
    void initialBoard(std::string setup);
    void setPiece(char pieceType, Square square);
    void setPiece(std::shared_ptr<Piece> piece, Square square);
    void deletePiece(int x, int y);

 public:
    sf::RectangleShape board[8][8];

    Board(float squareSize, std::string piecesSetup);

    std::shared_ptr<Piece> &getPiece(int x, int y);
    bool movePiece(std::shared_ptr<Piece> piece, Square square);
    void invertPosition();
    void setColor(sf::Color newColor);
    std::shared_ptr<Piece> createPiece(char piece, Square square);
    void paintMove(Square old, Square moved);
    void resetColors();
    bool getOrientation();
};
