#pragma once

#ifndef HEADERS_PIECE_H_
#define HEADERS_PIECE_H_

#include <vector>
#include <SFML/Graphics.hpp>

struct Square {
    int x;
    int y;
    float size;
};

typedef std::vector<Square> square_list;

class Piece {
private:
    sf::Texture* texture;
    sf::Sprite* sprite;
    char piece;
    Square square;
    int move = 0;

    void setTexture(char piece);
    void setSpritePosition(Square square);
    sf::Vector2f findSquarePosition(Square square);

public:
    Piece(char piece, Square square);
    virtual ~Piece() = default;

    bool isWhite();
    void drag(sf::Vector2f mousePosition);
    virtual square_list getMoves() = 0;
    void setSquare(Square square);
    Square getSquare();
    sf::Sprite& getSprite();
    char getType();
    virtual bool isValidMove(Square newSquare) = 0;
    void moved();
    void unmoved();
    bool hasMoved();
};

#endif  // HEADERS_PIECE_H_
