#include "Game.h"
#include "Pieces.h"
#include <SFML/System/Vector2.hpp>
#include <iostream>

static Square getSquare(const sf::Vector2i &position, float squareSize) {
    return {(int)(position.x / squareSize), (int)(position.y / squareSize), squareSize};
}

static bool sameSquare(Square &a, Square &b) { return a.x == b.x && a.y == b.y; }

Game::Game(sf::RenderWindow &window, float squareSize) : _window(window), squareSize(squareSize) {
    board = new Board(squareSize, initialBoard);
    sounds.playStart();
}

void Game::draw() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            _window.draw(board->board[i][j]);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece *piece = board->getPiece(i, j);
            if (piece != nullptr && piece != moving)
                _window.draw(piece->getSprite());
        }
    }

    if (moving != nullptr) {
        showAvailableSquares();
        _window.draw(moving->getSprite());
    }
}

bool Game::piecePressed(const sf::Vector2i &position) {
    int x = position.x / squareSize;
    int y = position.y / squareSize;

    Piece *piece = board->getPiece(x, y);
    if (piece == nullptr || !turnCheck(piece->getType()))
        return false;

    board->resetColors();
    moving = piece;

    piece = nullptr;
    delete piece;
    return true;
}

void Game::pieceDrag() {
    if (moving == nullptr)
        return;

    moving->drag(static_cast<sf::Vector2f>(sf::Mouse::getPosition(_window)));
}

bool Game::pieceReleased(const sf::Vector2i &position) {
    if (moving == nullptr)
        return false;

    Square pieceSquare = moving->getSquare();

    Square movingSquare = getSquare(position, squareSize);

    size_t length = moves.size();
    if (!isLegalMove(moving, &movingSquare)) {
        resetMoving();
        if (!sameSquare(pieceSquare, movingSquare))
            sounds.playIlegal();
        return false;
    }
    move_tuple move(moving, pieceSquare, board->getPiece(movingSquare.x, movingSquare.y), movingSquare,
                    board->getOrientation());
    moves.push(move);
    redoMoves = std::stack<move_tuple>();

    board->resetColors();
    board->paintMove(pieceSquare, movingSquare);

    bool captured = board->movePiece(moving, movingSquare);
    if (length != moves.size() - 1 && !captured)
        sounds.playCastle();
    else if (captured)
        sounds.playCapture();
    else
        sounds.playMove();

    moving = nullptr;
    toggleTurn();

    return true;
}

void Game::resetMoving() {
    if (moving == nullptr)
        return;

    board->movePiece(moving, moving->getSquare());
}

bool Game::isLegalMove(Piece *piece, Square *square) {
    int x = square->x;
    int y = square->y;

    int pieceX = piece->getSquare().x;
    int pieceY = piece->getSquare().y;

    if (x == pieceX && y == pieceY)
        return false;

    bool isValid;
    char pieceType = getPieceType(piece);
    int dX = pieceX - x;
    int dY = pieceY - y;

    Rook *rook = nullptr;
    Piece *newSquarePiece = board->getPiece(x, y);

    isValid = piece->isValidMove(*square);

    switch (pieceType) {
    case 'p': {
        Pawn *pawn = (Pawn *)piece;
        isValid = pawn->isValidMove(*square, board->getOrientation());

        if (!isValid)
            return false;

        if (abs(dX) == 1 && abs(dY) == 1 && newSquarePiece == nullptr) {
            Piece *enPassantPiece = board->getPiece(x, pieceY);

            if (enPassantPiece == nullptr || getPieceType(enPassantPiece) != 'p' ||
                !((Pawn *)enPassantPiece)->getEnPassant())
                return false;

            moves.push(
                move_tuple(enPassantPiece, enPassantPiece->getSquare(), nullptr, *square, board->getOrientation()));

            board->movePiece(enPassantPiece, *square);
        } else if (dX != 0 &&
                   (abs(dY) != 1 || newSquarePiece == nullptr || newSquarePiece->isWhite() == piece->isWhite())) {
            return false;
        } else if (dX == 0) {
            int orientationDir = board->getOrientation() ? 1 : -1;

            int i = pieceY;
            do {
                (piece->isWhite()) ? i -= 1 *orientationDir : i += 1 * orientationDir;
                if (board->getPiece(x, i) != nullptr)
                    return false;
            } while (i != y);
        }

        pawn->allowEnPassant(false);
        if (abs(dY) == 2)
            pawn->allowEnPassant(true);
        break;
    }
    case 'k':
        if (dX <= -2) {
            if (piece->isWhite())
                rook = (Rook *)board->getPiece(7, 7);
            else
                rook = (Rook *)board->getPiece(7, 0);
        } else if (dX >= 2) {
            if (piece->isWhite())
                rook = (Rook *)board->getPiece(0, 7);
            else
                rook = (Rook *)board->getPiece(0, 0);
        }

        if (!isValid)
            return false;

        // Will castle
        if (rook != nullptr) {
            if (rook->hasMoved())
                return false;
            int rX = rook->getSquare().x, rY = rook->getSquare().y;

            int castleRook = -2;
            dX = 2;

            if (rook->getSquare().x == 0) {
                castleRook = 3;
                dX = -2;
            }

            Square castledSquare{rX + castleRook, rY, squareSize};

            {
                int i = pieceX;

                do {
                    (dX > 0) ? i++ : i--;

                    newSquarePiece = board->getPiece(i, pieceY);

                    if (sameColorCapture(piece, newSquarePiece))
                        return false;
                } while (i != (pieceX + dX));

                // Needs guard check
            }

            move_tuple rookCastled(rook, rook->getSquare(), nullptr, castledSquare, board->getOrientation());
            moves.push(rookCastled);

            board->movePiece(rook, castledSquare);
            rook->moved();

            square->x = pieceX + dX;
        } else {
            if (sameColorCapture(piece, newSquarePiece))
                return false;
        }
        break;
    case 'n':
        if (!isValid)
            return false;

        if (sameColorCapture(piece, newSquarePiece))
            return false;
        break;
    default:
        if (!isValid)
            return false;

        {
            int i = pieceX, j = pieceY;

            do {
                if (dX != 0)
                    (dX < 0) ? i++ : i--;
                if (dY != 0)
                    (dY < 0) ? j++ : j--;

                newSquarePiece = board->getPiece(i, j);

                if (sameColorCapture(piece, newSquarePiece))
                    return false;
            } while (i != x || j != y);
        }
        break;
    }

    piece->moved();

    return true;
}

bool Game::turnCheck(char pieceType) {
    return (whiteTurn && std::isupper(pieceType)) || (!whiteTurn && std::islower(pieceType));
}

void Game::undo() {
    if (moves.empty())
        return;

    move_tuple lastMove(moves.top());
    moves.pop();

    Piece *movedPiece = std::get<0>(lastMove);
    Piece *takenPiece = std::get<2>(lastMove);

    Square oldSquare = std::get<1>(lastMove);
    Square newSquare = std::get<3>(lastMove);

    board->resetColors();

    bool sameOrientation = std::get<4>(lastMove) != board->getOrientation();
    if (sameOrientation)
        board->invertPosition();

    board->movePiece(movedPiece, oldSquare);
    if (takenPiece != nullptr)
        board->movePiece(takenPiece, newSquare);

    movedPiece->unmoved();

    if (sameOrientation)
        board->invertPosition();

    char pieceType = getPieceType(movedPiece);
    if (pieceType == 'k' && abs(oldSquare.x - newSquare.x) == 2) {
        sounds.playCastle();
        undo();
    } else if (pieceType == 'p' && !moves.empty()) {
        move_tuple enPassantMove(moves.top());

        Piece *lastPiece = std::get<0>(enPassantMove);
        Square oldSquare = std::get<1>(enPassantMove);
        Square newSquare = std::get<3>(enPassantMove);

        short direction = (lastPiece->isWhite() ? -1 : 1);
        if (getPieceType(lastPiece) == 'p' && (oldSquare.y - newSquare.y == 1 * direction)) {
            sounds.playCapture();
            undo();
        } else {
            toggleTurn();

            if (abs(oldSquare.y - newSquare.y) == 2)
                ((Pawn *)movedPiece)->allowEnPassant(false);
        }
    } else
        toggleTurn();

    if (takenPiece != nullptr)
        sounds.playCapture();
    else
        sounds.playMove();
    redoMoves.push(lastMove);
}

void Game::redo() {
    if (redoMoves.empty())
        return;

    move_tuple undoed(redoMoves.top());
    redoMoves.pop();

    Piece *movedPiece = std::get<0>(undoed);
    Square oldSquare = std::get<1>(undoed);
    Square newSquare = std::get<3>(undoed);

    board->resetColors();
    board->paintMove(oldSquare, newSquare);

    bool sameOrientation = std::get<4>(undoed) != board->getOrientation();
    if (sameOrientation)
        board->invertPosition();

    board->movePiece(movedPiece, newSquare);
    movedPiece->moved();

    if (sameOrientation)
        board->invertPosition();

    char pieceType = getPieceType(movedPiece);
    if (pieceType == 'k' && abs(oldSquare.x - newSquare.x) == 2) {
        sounds.playCastle();
        redo();
    } else if (pieceType == 'p' && !redoMoves.empty()) {
        move_tuple enPassantMove(redoMoves.top());

        Piece *lastPiece = std::get<0>(enPassantMove);
        Square oldSquare = std::get<1>(enPassantMove);
        Square newSquare = std::get<3>(enPassantMove);

        short direction = (lastPiece->isWhite() ? -1 : 1);
        if (getPieceType(lastPiece) == 'p' && (oldSquare.y - newSquare.y == 1 * direction)) {
            sounds.playCapture();
            redo();

            board->movePiece(movedPiece, newSquare);
        } else {
            toggleTurn();

            if (abs(oldSquare.y - newSquare.y) == 2)
                ((Pawn *)movedPiece)->allowEnPassant(true);
        }
    } else
        toggleTurn();

    if (std::get<2>(undoed) != nullptr)
        sounds.playCapture();
    else
        sounds.playMove();
    moves.push(undoed);
}

void Game::toggleTurn() {
    whiteTurn = !whiteTurn;
    // switchSides();
}

void Game::switchSides() { board->invertPosition(); }

void Game::showAvailableSquares() {
    square_list moves = moving->getMoves();
    circle_move circles;

    sf::Color gray = sf::Color(146, 146, 146, 146);
    for (Square square : moves) {
        sf::CircleShape circle(12.f);
        circle.setFillColor(gray);

        circle.setOrigin({12.f, 12.f});

        sf::Vector2f pos(square.x * square.size + square.size / 2, square.y * square.size + square.size / 2);
        circle.setPosition({pos.x, pos.y});

        circles.push_back(circle);
    }

    for (auto shape : circles) {
        _window.draw(shape);
    }
}

bool Game::pieceSelected() { return moving != nullptr; }

char Game::getPieceType(Piece *piece) { return std::tolower(piece->getType()); }

bool Game::sameColorCapture(Piece *moved, Piece *taken) {
    if (taken == nullptr)
        return false;

    if (moved->isWhite() != taken->isWhite())
        return false;

    return true;
}
