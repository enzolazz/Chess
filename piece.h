#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

class Piece {

private:
    sf::Texture* texture;
    sf::Sprite* sprite;
    sf::Vector2f position;
    char piece;

   void setTexture(char piece); 


public:
    Piece();
    Piece(char piece, sf::Vector2f position);

    void drag(sf::Vector2f mousePosition);
    void updatePosition(sf::Vector2f position);
    sf::Vector2f getPosition();
    sf::Sprite& getSprite();
    char getType();
};

Piece::Piece(char piece, sf::Vector2f position)
        : piece(piece), position(position) { 

    if (piece != '0') {
        setTexture(piece);
        sprite->setPosition({position.x, position.y});
    }
}


Piece::Piece() : piece('0'), position({0, 0}) {}

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

void Piece::drag(sf::Vector2f mousePosition) {
    sprite->setPosition({mousePosition.x, mousePosition.y});
}

void Piece::updatePosition(sf::Vector2f position) {
    this->position = position;
    sprite->setPosition({position.x, position.y});
}

sf::Vector2f Piece::getPosition() {
    return this->position;
}

sf::Sprite& Piece::getSprite() {
    return *sprite;
}

char Piece::getType() {
    return piece;
}

