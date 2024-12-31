#include "piece.h"
#include <vector>
#include <SFML/Graphics.hpp>

class Board {
private:
    sf::RenderWindow& _window;
    bool _isWhiteMove = true;
    std::string _initialBoard = "rnbqkbnrpppppppp8888PPPPPPPPRNBQKBNR";
    sf::Color whiteColor = sf::Color(240, 217, 181); // Branco clássico
    sf::Color blackColor = sf::Color(181, 136, 99);  // Marrom clássico

    float _squareSize;
    std::vector<std::vector<Piece>> pieces;
    Piece* _moving = nullptr;

    void updatePieces();
    void setPiece(int x, int y, char piece);
    void movePiece(const sf::Vector2i &position);
    bool isLegalMove(Piece piece, const sf::Vector2f &position);
    bool turnCheck(char pieceType);
public:
    sf::RectangleShape squares[8][8];

    Board(sf::RenderWindow& window, float squareSize);
    void draw();
    bool isWhiteMove() const { return _isWhiteMove; }
    void toggleTurn() { _isWhiteMove = !_isWhiteMove; }
    bool piecePressed(const sf::Vector2i &position);
    void pieceReleased();
    void pieceDrag();
    Piece* getPiece(int x, int y) { return &pieces[x][y]; }
};

Board::Board(sf::RenderWindow& window, float squareSize) : _window(window), _squareSize(squareSize) {
    pieces.resize(8, std::vector<Piece>(8, Piece('0', {0, 0})));

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            squares[i][j].setSize({squareSize, squareSize});
            squares[i][j].setPosition({i * squareSize, j * squareSize});

            sf::Color color = (i + j) % 2 == 0 ? whiteColor : blackColor;
            squares[i][j].setFillColor(color);
        }
    }

    updatePieces();
}

void Board::updatePieces() {
    int pieceIndex = 0;
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            char piece = _initialBoard[pieceIndex++];

            if (std::isdigit(piece)) {
                int spaces = piece - '0';
                i += spaces - 1;
            } else
                setPiece(i, j, piece);
        }
    }
}

void Board::setPiece(int x, int y, char piece) {
    pieces[x][y] = Piece(piece, {x * _squareSize + _squareSize / 2, y * _squareSize + _squareSize / 2});
}

void Board::draw() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            _window.draw(squares[i][j]);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = getPiece(i, j);
            if (piece->getType() != '0' && piece != this->_moving)
                _window.draw(piece->getSprite());
        }
    }

    if (this->_moving != nullptr)
        _window.draw(this->_moving->getSprite());
}

bool Board::piecePressed(const sf::Vector2i &position) {
    int x = position.x / _squareSize;
    int y = position.y / _squareSize;

    this->_moving = getPiece(x, y);
    char pieceType = this->_moving->getType();
    if (pieceType == '0' || !turnCheck(pieceType)) {
        this->_moving = nullptr;
        return false;
    }

    std::cout << "Clicked piece " << pieceType << " at (" << x << ", " << y << ")" << std::endl;
    return true;
}

void Board::pieceDrag() {
    if (this->_moving == nullptr) return;

    this->_moving->drag(static_cast<sf::Vector2f>(sf::Mouse::getPosition(_window)));
}

void Board::movePiece(const sf::Vector2i &position) {
    if (this->_moving == nullptr) return;
    
    Piece piece = *this->_moving;

    int oldX = piece.getPosition().x / _squareSize;
    int oldY = piece.getPosition().y / _squareSize;

    setPiece(oldX, oldY, '0');

    int x = position.x / _squareSize;
    int y = position.y / _squareSize;

    setPiece(x, y, piece.getType());
}

void Board::pieceReleased() {
    sf::Vector2f newPosition = this->_moving->getSprite().getPosition();

    if (!isLegalMove(*this->_moving, newPosition))
        newPosition = this->_moving->getPosition();
    else
        toggleTurn();

    movePiece(static_cast<sf::Vector2i>(newPosition));

    this->_moving = nullptr;
}

bool Board::isLegalMove(Piece piece, const sf::Vector2f &position) {
    int x = position.x / _squareSize;
    int y = position.y / _squareSize;

    int pieceX = piece.getPosition().x / _squareSize;
    int pieceY = piece.getPosition().y / _squareSize;

    if (x == pieceX && y == pieceY) return false;

    return true;
}

bool Board::turnCheck(char pieceType) {
    return (_isWhiteMove && std::isupper(pieceType)) || (!_isWhiteMove && std::islower(pieceType));
}
