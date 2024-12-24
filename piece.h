#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

class Piece {

private:
    sf::Texture* texture;
    sf::Sprite* sprite;

    void setTexture(char piece) {
        std::string path = "images/" + std::string(1, piece) + ".png";

        texture = new sf::Texture();
        if (!texture->loadFromFile(path)) {
            std::cerr << "Failed to load texture from: " << path << std::endl;  // Error message
            throw std::runtime_error("Falha ao carregar textura: " + path);
        }

        sprite = new sf::Sprite(*texture); 
    }


public:
    char piece;
    std::pair<float, float> position;

    Piece() : piece('0'), position({0, 0}) {}

    Piece(char piece, std::pair<float, float> position)
        : piece(piece), position(position) { 

        if (piece != '0') {
            setTexture(piece);
            sprite->setPosition({position.first, position.second});
            sprite->scale({1.5f, 1.5f});
        }
    }

    void updatePosition(std::pair<float, float> position) {
        this->position = position;
        sprite->setPosition({position.first, position.second});
    }

    const sf::Sprite& getSprite() const {
        return *sprite;
    }
};

