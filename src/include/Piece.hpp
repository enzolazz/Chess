#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

struct Square {
    int x;
    int y;
    float size;
};

typedef std::vector<Square> square_list;

class Piece {
 private:
    std::unique_ptr<sf::Texture> texture;
    std::unique_ptr<sf::Sprite> sprite;
    char piece;
    Square square;
    int move = 0;

    void setTexture(char piece);
    void setSpritePosition(Square square);
    sf::Vector2f findSquarePosition(Square square);

 public:
    Piece() = default;
    Piece(char piece, Square square);
    virtual ~Piece() = default;

    bool isWhite();
    void drag(sf::Vector2f mousePosition);
    virtual square_list getMoves() = 0;
    void setSquare(Square square);
    Square getSquare();
    sf::Sprite &getSprite();
    char getType();
    virtual bool isValidMove(Square newSquare) = 0;
    void moved();
    void unmoved();
    bool hasMoved();
};
