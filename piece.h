#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iostream>

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

    void setTexture(char piece); 
    void setSpritePosition(Square square);
    sf::Vector2f findSquarePosition(Square square);

public:
    Piece(char piece, Square square);

    bool isWhite();
    void drag(sf::Vector2f mousePosition);
    virtual square_list getMoves() = 0;
    void setSquare(Square square);
    Square getSquare();
    sf::Sprite& getSprite();
    char getType();
    virtual bool isValidMove(Square newSquare, bool pieceCondition) = 0;
    virtual bool isValidMove(Square newSquare) = 0;
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

class Pawn : public Piece {
private:
    bool isInInitialRow(bool boardOrientation);
public:
    explicit Pawn(char piece, Square square) : Piece(piece, square) {
    }

    square_list getMoves();
    bool isValidMove(Square newSquare, bool boardOrientation); 
    bool isValidMove(Square newSquare) { return false; }

};

class Rook : public Piece {
private:
    bool hasMoved = false;
public:
    explicit Rook(char piece, Square square) : Piece(piece, square) {
    }

    square_list getMoves();
    bool isValidMove(Square newSquare); 
    bool isValidMove(Square newSquare, bool boardOrientation) { return false; }
    bool hasRookMoved() { return hasMoved; }
};

class Bishop: public Piece {
public:
    explicit Bishop(char piece, Square square) : Piece(piece, square) {
    }

    square_list getMoves();
    bool isValidMove(Square newSquare); 
    bool isValidMove(Square newSquare, bool boardOrientation) { return false; }
};

class Knight: public Piece {
public:
    explicit Knight(char piece, Square square) : Piece(piece, square) {
    }

    square_list getMoves();
    bool isValidMove(Square newSquare); 
    bool isValidMove(Square newSquare, bool boardOrientation) { return false; }
};

class King: public Piece {
private:
    bool hasMoved = false;
public:
    explicit King(char piece, Square square) : Piece(piece, square) {
    }

    void moved();
    void unmoved();
    square_list getMoves();
    bool isValidMove(Square newSquare); 
    bool isValidMove(Square newSquare, bool hasRook); 
};

class Queen: public Piece {
public:
    explicit Queen(char piece, Square square) : Piece(piece, square) {
    }

    square_list getMoves();
    bool isValidMove(Square newSquare); 
    bool isValidMove(Square newSquare, bool boardOrientation) { return false; }
};


bool Pawn::isInInitialRow(bool boardOrientation)  {
    int row = getSquare().y + 1;
    int initial;

    if (isWhite()) {
        if (boardOrientation) initial = 7;
        else initial = 2;
    } else {
        if (boardOrientation) initial = 2;
        else initial = 7;
    }

    return initial == row;
}

bool Pawn::isValidMove(Square newSquare, bool boardOrientation) {
    bool initial = isInInitialRow(boardOrientation);

    int walk = (initial ? 2 : 1) * (boardOrientation ? 1 : -1);
    int moveX = getSquare().x - newSquare.x, moveY = getSquare().y - newSquare.y;

    if(!isWhite()) moveY *= -1;

    if ( (moveY > 0 && moveY <= walk) && (moveX >= -1 && moveX <= 1) )
        return true;

    return false;
}

bool Rook::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;

    if (abs(x - newSquare.x) > 0 && (y - newSquare.y == 0) ||
        abs(y - newSquare.y) > 0 && (x - newSquare.x == 0))
        return true;

    return false;
} 

bool Knight::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;
    int moveX = abs(x - newSquare.x), moveY = abs(y - newSquare.y);

    if ((moveY == 2 && moveX == 1) || (moveX == 2 && moveY == 1))
        return true;

    return false;
}
 
bool Bishop::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;

    if (abs(x - newSquare.x) == abs(y - newSquare.y))
        return true;

    return false;
}


bool King::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;

    if (abs(x - newSquare.x) <= 1 && abs(y - newSquare.y) <= 1)
        return true;

    return false;
}

bool King::isValidMove(Square newSquare, bool hasRookMoved) {
    int x = getSquare().x, y = getSquare().y;

    if ((y - newSquare.y == 0) && ((x - newSquare.x >= 2) || (x - newSquare.x <= -2)))
        return !hasMoved;

    return false;
}

void King::moved() {
    hasMoved = true;
}

void King::unmoved() {
    hasMoved = false;
}

bool Queen::isValidMove(Square newSquare) {
    int x = getSquare().x, y = getSquare().y;

    if (abs(x - newSquare.x) == abs(y - newSquare.y))
        return true;

    if (abs(x - newSquare.x) > 0 && (y - newSquare.y == 0) ||
        abs(y - newSquare.y) > 0 && (x - newSquare.x == 0))
        return true;

    return false;
}

square_list Pawn::getMoves() {
    square_list s;

    s.push_back(Square{3, 3, getSquare().size});
    s.push_back(Square{3, 4, getSquare().size});
    s.push_back(Square{3, 5, getSquare().size});

    return s;
}

square_list Bishop::getMoves() {
    return std::vector<Square>();
}

square_list Knight::getMoves() {
    return std::vector<Square>();
}

square_list Rook::getMoves() {
    return std::vector<Square>();
}

square_list King::getMoves() {
    return std::vector<Square>();
}

square_list Queen::getMoves() {
    return std::vector<Square>();
}

