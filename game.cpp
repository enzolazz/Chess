#include "board.cpp"
#include <vector>
#include <tuple>
#include <stack>
#include <SFML/Graphics.hpp>

typedef std::tuple<Piece*, Square, Piece*, Square, bool> move_tuple;

class Game {
private:
    sf::RenderWindow& _window;
    float _squareSize;
    bool _isWhiteMove = true;
    std::string _initialBoard = "rnbqkbnrpppppppp8888PPPPPPPPRNBQKBNR";
    Board* board;
    Piece* _moving = nullptr;
    std::stack<move_tuple> moves;
    std::stack<move_tuple> redoMoves;

    bool isLegalMove(Piece* piece, Square* newSquare);
    bool turnCheck(char pieceType);
    void toggleTurn();
public:
    Game(sf::RenderWindow& window, float squareSize);

    void draw();
    bool isWhiteMove() const { return _isWhiteMove; }
    bool piecePressed(const sf::Vector2i &position);
    void pieceReleased();
    void pieceDrag();
    void undo();
    void redo();
    void showAvailableSquares(Piece* piece);
    void switchSides();
};

Game::Game(sf::RenderWindow& window, float squareSize) : _window(window), _squareSize(squareSize) {
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

    //std::cout << "Clicked piece " << _moving->getType() << " at (" << x << ", " << y << ")" << std::endl;
    return true;
}

void Game::pieceDrag() {
    if (_moving == nullptr) return;

    _moving->drag(static_cast<sf::Vector2f>(sf::Mouse::getPosition(_window)));
}

void Game::pieceReleased() {
    Square oldSquare = _moving->getSquare();

    sf::Vector2f newPosition = _moving->getSprite().getPosition();
    Square movingSquare =  {(int) (newPosition.x / _squareSize),
                            (int) (newPosition.y / _squareSize), _squareSize};

    bool legalMove = isLegalMove(_moving, &movingSquare);
    if (!legalMove)
        movingSquare = oldSquare;
    else {
        move_tuple move (_moving, oldSquare,
                                   board->getPiece(movingSquare.x, movingSquare.y), movingSquare,
                                   board->getOrientation());
        moves.push(move);
        redoMoves = std::stack<move_tuple>();

        board->resetColors();
        board->paintMove(oldSquare, movingSquare);
    }

    board->movePiece(_moving, movingSquare);

    _moving = nullptr;

    if (legalMove) toggleTurn();
}

bool Game::isLegalMove(Piece* piece, Square* square){
    int x = square->x;
    int y = square->y;

    int pieceX = piece->getSquare().x;
    int pieceY = piece->getSquare().y;

    if (x == pieceX && y == pieceY)
        showAvailableSquares(piece);
    else {
        bool isValid;
        int dX = pieceX - x;

        Rook* rook = nullptr;
        switch (std::tolower(piece->getType())) {
            case 'p':
                isValid = piece->isValidMove(*square, board->getOrientation());
                break;
            case 'k':
                if (dX <= -2) {
                    if (piece->isWhite()) rook = (Rook*) board->getPiece(7, 7);
                    else rook = (Rook*) board->getPiece(7, 0);
                } else if (dX >= 2) {
                    if (piece->isWhite()) rook = (Rook*) board->getPiece(0, 7);
                    else rook = (Rook*) board->getPiece(0, 0);
                }

                if (rook == nullptr) isValid = piece->isValidMove(*square);
                else isValid = piece->isValidMove(*square, rook->hasRookMoved());
                break;
            default:
                isValid = piece->isValidMove(*square);
                break;
        }
        if (isValid) {
        // check if piece can move there.
        // if all checks, return true;
            if (std::tolower(piece->getType()) == 'k') {
                King* king = (King*) piece;
                king->moved();
            }

            if (rook != nullptr) {
                int rX = rook->getSquare().x, rY = rook->getSquare().y;

                int castleRook = -2;
                dX = 2;
                if (rook->getSquare().x == 0) {
                    castleRook = 3;
                    dX = -2;
                }
                Square castledSquare{rX + castleRook, rY, _squareSize};

                move_tuple rookCastled(rook, rook->getSquare(), nullptr, castledSquare, board->getOrientation());
                moves.push(rookCastled);

                board->movePiece(rook, castledSquare);

                square->x = pieceX + dX;
            }
            return true;
        }
    }

    return false;
}

bool Game::turnCheck(char pieceType) {
    return (_isWhiteMove && std::isupper(pieceType)) || (!_isWhiteMove && std::islower(pieceType));
}

void Game::undo() {
    if (moves.empty()) return;

    move_tuple lastMove (moves.top());
    moves.pop();

    Piece* movedPiece = std::get<0>(lastMove);
    Piece* takenPiece = std::get<2>(lastMove);

    Square oldSquare = std::get<1>(lastMove);
    Square newSquare = std::get<3>(lastMove);

    board->resetColors();

    bool sameOrientation = std::get<4>(lastMove) != board->getOrientation();
    if (sameOrientation) board->invertPosition();

    board->movePiece(movedPiece, oldSquare);
    if (takenPiece != nullptr) board->movePiece(takenPiece, newSquare);

    if (sameOrientation) board->invertPosition();

    if (std::tolower(movedPiece->getType()) == 'k' && abs(oldSquare.x - newSquare.x) == 2) {
        ((King*) movedPiece)->unmoved();
        undo();
    } else toggleTurn();

    redoMoves.push(lastMove);
}

void Game::redo() {
    if (redoMoves.empty()) return;

    move_tuple undoed (redoMoves.top());
    redoMoves.pop();

    Piece* movedPiece = std::get<0>(undoed);
    Square oldSquare = std::get<1>(undoed);
    Square newSquare = std::get<3>(undoed);

    board->resetColors();
    board->paintMove(oldSquare, newSquare);

    bool sameOrientation = std::get<4>(undoed) != board->getOrientation();
    if (sameOrientation) board->invertPosition();

    board->movePiece(movedPiece, newSquare);

    if (sameOrientation) board->invertPosition();

    if (std::tolower(movedPiece->getType()) == 'k' && abs(oldSquare.x - newSquare.x) == 2) {
        ((King*) movedPiece)->moved();
        redo();
    } else toggleTurn();

    moves.push(undoed);
}

void Game::toggleTurn() { 
    _isWhiteMove = !_isWhiteMove;
    //switchSides();
}

void Game::switchSides() {
   board->invertPosition(); 
}

void Game::showAvailableSquares(Piece* piece) {
    std::cout << "Imagine available squares :)\n";
}
