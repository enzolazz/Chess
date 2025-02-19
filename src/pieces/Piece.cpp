#include <iostream>
#include "../../headers/pieces/Piece.h"

Piece::Piece(char piece, Square square)
    : piece(piece), square(square) {

    if (piece != '0') {
        setTexture(piece);
        setSpritePosition(square);
    }
}

void Piece::setTexture(char piece) {
    std::string path = "images/" + std::string(1, piece) + ".png";

    texture = new sf::Texture();
    if (!texture->loadFromFile(path)) {
        std::cerr << "Failed to load texture from: " << path << std::endl;  // Error message
        throw std::runtime_error("Falha ao carregar textura: " + path);
    }

    sprite = new sf::Sprite(*texture);
    sprite->setOrigin({
        static_cast<float>(texture->getSize().x / 2),
        static_cast<float>(texture->getSize().y / 2)
    });
}

bool Piece::isWhite() {
    return std::isupper(getType());
}
void Piece::drag(sf::Vector2f mousePosition) {
    sprite->setPosition(mousePosition);
}

void Piece::setSquare(Square square) {
    this->square = square;

    setSpritePosition(square);
}

Square Piece::getSquare() {
    return square;
}

void Piece::setSpritePosition(Square square) {
    sf::Vector2f position = findSquarePosition(square);
    sprite->setPosition({position.x, position.y});
}

sf::Sprite& Piece::getSprite() {
    return *sprite;
}

char Piece::getType() {
    return piece;
}

sf::Vector2f Piece::findSquarePosition(Square square) {
    sf::Vector2f pos(square.x * square.size + square.size / 2, square.y * square.size + square.size / 2);
    return pos;
}

void Piece::moved() {
    move++;
}

void Piece::unmoved() {
    move--;
}

bool Piece::hasMoved() {
    return move != 0;
}
