#include "board.cpp"
#include <vector>
#include <tuple>
#include <stack>
#include <SFML/Graphics.hpp>

typedef std::tuple<Piece*, Square, Piece*, Square, bool> move_tuple;
typedef std::vector<sf::CircleShape> circle_move;

class Game {
private:
    sf::RenderWindow& _window;
    float squareSize;
    bool whiteTurn = true;
    std::string initialBoard = "rnbqkbnrpppppppp8888PPPPPPPPRNBQKBNR";
    Board* board;
    Piece* moving = nullptr;
    std::stack<move_tuple> moves;
    std::stack<move_tuple> redoMoves;

    bool isLegalMove(Piece* piece, Square* newSquare);
    bool turnCheck(char pieceType);
    void toggleTurn();
    char getPieceType(Piece* piece);
public:
    Game(sf::RenderWindow& window, float squareSize);

    void draw();
    bool isWhiteMove() const { return whiteTurn; }
    bool piecePressed(const sf::Vector2i &position);
    void pieceReleased();
    void pieceDrag();
    void undo();
    void redo();
    void showAvailableSquares();
    bool pieceSelected();
    void switchSides();
};

Game::Game(sf::RenderWindow& window, float squareSize) : _window(window), squareSize(squareSize) {
    board = new Board(squareSize, initialBoard);
}

void Game::draw() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            _window.draw(board->board[i][j]);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece* piece = board->getPiece(i, j);
            if (piece != nullptr && piece != moving)
                _window.draw(piece->getSprite());
        }
    }

    if (moving != nullptr)
    {
        showAvailableSquares();
        _window.draw(moving->getSprite());
    }
}

bool Game::piecePressed(const sf::Vector2i &position) {
    int x = position.x / squareSize;
    int y = position.y / squareSize;

    moving = board->getPiece(x, y);
    if (moving == nullptr || !turnCheck(moving->getType())) {
        moving = nullptr;
        return false;
    }

    board->resetColors();

    //std::cout << "Clicked piece " << moving->getType() << " at (" << x << ", " << y << ")" << std::endl;
    return true;
}

void Game::pieceDrag() {
    if (moving == nullptr) return;

    moving->drag(static_cast<sf::Vector2f>(sf::Mouse::getPosition(_window)));
}

void Game::pieceReleased() {
    Square oldSquare = moving->getSquare();

    sf::Vector2f newPosition = moving->getSprite().getPosition();
    Square movingSquare =  {(int) (newPosition.x / squareSize),
                            (int) (newPosition.y / squareSize), squareSize};

    bool legalMove = isLegalMove(moving, &movingSquare);
    if (!legalMove)
        movingSquare = oldSquare;
    else {
        move_tuple move (moving, oldSquare,
                                   board->getPiece(movingSquare.x, movingSquare.y), movingSquare,
                                   board->getOrientation());
        moves.push(move);
        redoMoves = std::stack<move_tuple>();

        board->resetColors();
        board->paintMove(oldSquare, movingSquare);
    }

    board->movePiece(moving, movingSquare);

    if (legalMove) 
    {
        moving = nullptr;
        toggleTurn();
    }
}

bool Game::isLegalMove(Piece* piece, Square* square){
    int x = square->x;
    int y = square->y;

    int pieceX = piece->getSquare().x;
    int pieceY = piece->getSquare().y;

    if (x != pieceX || y != pieceY)
    {
        bool isValid;
        int dX = pieceX - x;
        int dY = pieceY - y;

        Rook* rook = nullptr;
        Piece* newSquarePiece = board->getPiece(x, y);
        switch (getPieceType(piece))
        {
            case 'p':
                isValid = piece->isValidMove(*square, board->getOrientation());

                std::cout << isValid << std::endl;
                if (!isValid) return false;

                if (dX != 0 && abs(dY) == 1 && 
                    (newSquarePiece == nullptr || 
                    newSquarePiece->isWhite() == piece->isWhite()))
                        return false;

                if (dX == 0)
                {
                    int orientationDir = board->getOrientation() ? 1 : -1;

                    int i = pieceY;
                    do
                    {
                        (piece->isWhite()) ? i -= 1 * orientationDir : i += 1 * orientationDir;
                        if (board->getPiece(x, i) != nullptr) return false;
                    } while (i != y);

                }
                break;
            case 'k':
                if (dX <= -2)
                {
                    if (piece->isWhite()) rook = (Rook*) board->getPiece(7, 7);
                    else rook = (Rook*) board->getPiece(7, 0);
                } else if (dX >= 2)
                {
                    if (piece->isWhite()) rook = (Rook*) board->getPiece(0, 7);
                    else rook = (Rook*) board->getPiece(0, 0);
                }

                if (rook == nullptr) isValid = piece->isValidMove(*square);
                else isValid = piece->isValidMove(*square, rook->hasRookMoved());

                if (!isValid) return false;

                ((King*) piece)->moved();

                // Will castle
                if (rook != nullptr) {
                    int rX = rook->getSquare().x, rY = rook->getSquare().y;

                        int castleRook = -2;
                    dX = 2;
                    if (rook->getSquare().x == 0) 
                    {
                        castleRook = 3;
                        dX = -2;
                    }

                    Square castledSquare{rX + castleRook, rY, squareSize};

                    move_tuple rookCastled(rook, rook->getSquare(), nullptr, castledSquare, board->getOrientation());
                    moves.push(rookCastled);

                    board->movePiece(rook, castledSquare);

                    square->x = pieceX + dX;
                }
                break;
            default:
                isValid = piece->isValidMove(*square);
                break;
        }

            return true;
    }

    return false;
}

bool Game::turnCheck(char pieceType) {
    return (whiteTurn && std::isupper(pieceType)) || (!whiteTurn && std::islower(pieceType));
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

    if (getPieceType(movedPiece) == 'k' && abs(oldSquare.x - newSquare.x) == 2) {
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

    if (getPieceType(movedPiece) == 'k' && abs(oldSquare.x - newSquare.x) == 2) {
        ((King*) movedPiece)->moved();
        redo();
    } else toggleTurn();

    moves.push(undoed);
}

void Game::toggleTurn() { 
    whiteTurn = !whiteTurn;
    //switchSides();
}

void Game::switchSides() {
   board->invertPosition(); 
}

void Game::showAvailableSquares() {
    square_list moves = moving->getMoves();
    circle_move circles;

    sf::Color gray = sf::Color(146, 146, 146, 146);
    for (Square square : moves) 
    {
        sf::CircleShape circle(12.f);
        circle.setFillColor(gray);

        circle.setOrigin({12.f, 12.f});

        sf::Vector2f pos(square.x * square.size + square.size / 2, square.y * square.size + square.size / 2);
        circle.setPosition({pos.x, pos.y});

        circles.push_back(circle);
    }

    for (auto shape : circles)
    {
        _window.draw(shape);
    }
}

bool Game::pieceSelected() {
    return moving != nullptr;
}

char Game::getPieceType(Piece* piece) {
    return std::tolower(piece->getType()); 
}
