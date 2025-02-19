#pragma once

#ifndef HEADERS_BOARD_H_
#define HEADERS_BOARD_H_

#include <vector>
#include <utility>
#include <string>
#include <SFML/Graphics.hpp>
#include "pieces/Piece.h"

class Board {
private:
    sf::Color whiteColor = sf::Color(240, 217, 181); // Branco clássico
    sf::Color blackColor = sf::Color(181, 136, 99);  // Marrom clássico
    sf::Color moveColor = sf::Color(237, 234, 83);  // Marrom clássico

    float squareSize = 0;
    std::vector<std::vector<Piece*>> pieces = std::vector<std::vector<Piece*>> (8, std::vector<Piece*>(8));
    bool orientation = true;

    sf::Color findColor(int i, int j);
    void initialBoard(std::string setup);
    void setPiece(char pieceType, Square square);
    void setPiece(Piece *piece, Square square);
    template <typename T> void swap(T*& a, T*& b);
    void deletePiece(int x, int y);
public:
    sf::RectangleShape board[8][8];

    Board(float squareSize, std::string piecesSetup);
    Piece* getPiece(int x, int y);
    void movePiece(Piece* piece, Square square);
    void invertPosition();
    void setColor(sf::Color newColor);
    Piece* createPiece(char piece, Square square);
    void paintMove(Square old, Square moved);
    void resetColors();
    bool getOrientation();
};

#endif  // HEADERS_BOARD_H_
