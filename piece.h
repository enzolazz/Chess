#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

struct Square {
    int x;
    int y;
    float size;
};

class Piece {

private:
    sf::Texture* texture;
    sf::Sprite* sprite;
    char piece;
    Square square;

    void setTexture(char piece); 
    void setSpritePosition(Square square);
    sf::Vector2f findSquarePosition(Square square);

public:
    bool orientation = true;

    Piece(char piece, Square square);

    void drag(sf::Vector2f mousePosition);
    void setSquare(Square square);
    Square getSquare();
    sf::Sprite& getSprite();
    char getType();
    virtual bool isValidMove(Square newSquare) = 0;
    void toggleOrientation();
};

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

void Piece::toggleOrientation() {
    orientation = !orientation;
}

class Pawn : public Piece {
private:
    bool isInInitialRow() {
        int row = getSquare().y + 1;
        if (std::isupper(getType())) return row == orientation ? 7 : 2;
        else return row == orientation ? 2 : 7;
    }

public:
    explicit Pawn(char piece, Square square) : Piece(piece, square) {}

    bool isValidMove(Square newSquare) override {
        std::cout << "Entrou no peão\n";
        int walk = (isInInitialRow() ? 2 : 1) * (orientation ? 1 : -1);
        int moveX = getSquare().x - newSquare.x, moveY = getSquare().y - newSquare.y;

        if (moveY == 0 || moveX > 1) return false;

        if (orientation) {
            if (moveY > walk) return false;
        } else 
            if (moveY < walk) return false;

        return true;
    }
};

class Rook : public Piece {
public:
    explicit Rook(char piece, Square square) : Piece(piece, square) {}

    bool isValidMove(Square newSquare) {return true;}
};

class Bishop: public Piece {
public:
    explicit Bishop(char piece, Square square) : Piece(piece, square) {}

    bool isValidMove(Square newSquare) {return true;}
};

class Knight: public Piece {
public:
    explicit Knight(char piece, Square square) : Piece(piece, square) {}

    bool isValidMove(Square newSquare) {return true;}
};

class King: public Piece {
public:
    explicit King(char piece, Square square) : Piece(piece, square) {}

    bool isValidMove(Square newSquare) {return true;}
};

class Queen: public Piece {
public:
    explicit Queen(char piece, Square square) : Piece(piece, square) {}

    bool isValidMove(Square newSquare) {return true;}
};





