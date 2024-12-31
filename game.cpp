#include "board.cpp"
#include <vector>
#include <SFML/Graphics.hpp>

class Game {
private:
    sf::RenderWindow& _window;
    int _squareSize;
    bool _isWhiteMove = true;
    std::string _initialBoard = "rnbqkbnrpppppppp8888PPPPPPPPRNBQKBNR";
    Board* board;
    Piece* _moving = nullptr;

    bool isLegalMove(Piece piece, const sf::Vector2f &position);
    bool turnCheck(char pieceType);
    void toggleTurn() { _isWhiteMove = !_isWhiteMove; }
public:
    Game(sf::RenderWindow& window, int squareSize);

    void draw();
    bool isWhiteMove() const { return _isWhiteMove; }
    bool piecePressed(const sf::Vector2i &position);
    void pieceReleased();
    void pieceDrag();
};

Game::Game(sf::RenderWindow& window, int squareSize) : _window(window), _squareSize(squareSize) {
    board = new Board(squareSize, _initialBoard);
}

void Game::draw() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            _window.draw(board->board[i][j]);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board->getPiece(i, j);
            if (piece != nullptr && piece != _moving)
                _window.draw(piece->getSprite());
        }
    }

    if (_moving != nullptr)
        _window.draw(_moving->getSprite());
}

bool Game::piecePressed(const sf::Vector2i &position) {
    int x = position.x / _squareSize;
    int y = position.y / _squareSize;

    _moving = board->getPiece(x, y);
    if (_moving == nullptr || !turnCheck(_moving->getType())) {
        _moving = nullptr;
        return false;
    }

    std::cout << "Clicked piece " << _moving->getType() << " at (" << x << ", " << y << ")" << std::endl;
    return true;
}

void Game::pieceDrag() {
    if (_moving == nullptr) return;

    _moving->drag(static_cast<sf::Vector2f>(sf::Mouse::getPosition(_window)));
}

void Game::pieceReleased() {
    sf::Vector2f newPosition = _moving->getSprite().getPosition();

    if (!isLegalMove(*_moving, newPosition))
        newPosition = _moving->getPosition();
    else
        toggleTurn();

    board->movePiece(_moving, static_cast<sf::Vector2i>(newPosition));

    _moving = nullptr;
}

bool Game::isLegalMove(Piece piece, const sf::Vector2f &position) {
    int x = position.x / _squareSize;
    int y = position.y / _squareSize;

    int pieceX = piece.getPosition().x / _squareSize;
    int pieceY = piece.getPosition().y / _squareSize;

    if (x == pieceX && y == pieceY) return false;

    return true;
}

bool Game::turnCheck(char pieceType) {
    return (_isWhiteMove && std::isupper(pieceType)) || (!_isWhiteMove && std::islower(pieceType));
}
