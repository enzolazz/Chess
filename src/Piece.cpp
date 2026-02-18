#include "Piece.hpp"
#include "Board.hpp"
#include <iostream>

Piece::Piece(char piece, Square square) : piece(piece), square(square) {

    if (piece != '0') {
        setTexture(piece);
        setSpritePosition(square);
    }
}

void Piece::setTexture(char piece) {
    std::string path = "etc/images/" + std::string(1, piece) + ".png";

    texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(path)) {
        std::cerr << "Failed to load texture from: " << path << std::endl;
        throw std::runtime_error("Failed to load texture: " + path);
    }

    sprite = std::make_unique<sf::Sprite>(*texture);
    sprite->setOrigin({static_cast<float>(texture->getSize().x / 2), static_cast<float>(texture->getSize().y / 2)});
}

bool Piece::isWhite() { return std::isupper(getType()); }
void Piece::drag(sf::Vector2f mousePosition) { sprite->setPosition(mousePosition); }

void Piece::setSquare(Square square) {
    this->square = square;

    setSpritePosition(square);
}

Square Piece::getSquare() { return square; }

void Piece::setSpritePosition(Square square) {
    sf::Vector2f position = findSquarePosition(square);
    sprite->setPosition({position.x, position.y});
}

sf::Sprite &Piece::getSprite() { return *sprite; }

char Piece::getType() { return piece; }

sf::Vector2f Piece::findSquarePosition(Square square) {
    sf::Vector2f pos(BOARD_MARGIN + square.x * square.size + square.size / 2,
                     BOARD_MARGIN + square.y * square.size + square.size / 2);
    return pos;
}

void Piece::moved() { move++; }

void Piece::unmoved() { move--; }

bool Piece::hasMoved() { return move != 0; }
