#include "Board.hpp"
#include "Bishop.hpp"
#include "King.hpp"
#include "Knight.hpp"
#include "Pawn.hpp"
#include "Queen.hpp"
#include "Rook.hpp"

Board::Board(float squareSize, std::string piecesSetup)
    : squareSize(squareSize), orientation(true), pieces(8, std::vector<std::shared_ptr<Piece>>(8)) {
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            board[i][j].setSize({squareSize, squareSize});
            board[i][j].setPosition({BOARD_MARGIN + i * squareSize, BOARD_MARGIN + j * squareSize});

            sf::Color color = findColor(i, j);
            board[i][j].setFillColor(color);
        }
    }

    initialBoard(piecesSetup);
}

sf::Color Board::findColor(int i, int j) { return (i + j) % 2 == 0 ? WHITECOLOR : BLACKCOLOR; }

void Board::initialBoard(std::string piecesSetup) {
    int pieceIndex = 0;
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            char piece = piecesSetup[pieceIndex++];

            if (std::isdigit(piece)) {
                int spaces = piece - '0';
                i += spaces - 1;
            } else if (piece == '0')
                pieces[i][j] = nullptr;
            else
                setPiece(piece, Square{i, j, squareSize});
        }
    }
}

std::shared_ptr<Piece> &Board::getPiece(int x, int y) { return pieces[x][y]; }

std::shared_ptr<Piece> Board::createPiece(char piece, Square square) {
    switch (std::tolower(piece)) {
    case 'p':
        return std::make_unique<Pawn>(piece, square);
    case 'r':
        return std::make_unique<Rook>(piece, square);
    case 'n':
        return std::make_unique<Knight>(piece, square);
    case 'b':
        return std::make_unique<Bishop>(piece, square);
    case 'k':
        return std::make_unique<King>(piece, square);
    case 'q':
        return std::make_unique<Queen>(piece, square);
    default:
        return nullptr;
    }
}
void Board::setPiece(char pieceType, Square square) { pieces[square.x][square.y] = createPiece(pieceType, square); }

void Board::setPiece(std::shared_ptr<Piece> piece, Square square) {
    piece->setSquare(square);
    pieces[square.x][square.y] = std::move(piece);
}

void Board::deletePiece(int x, int y) { pieces[x][y] = nullptr; }

bool Board::movePiece(std::shared_ptr<Piece> piece, Square square) {
    int oldX = piece->getSquare().x;
    int oldY = piece->getSquare().y;

    deletePiece(oldX, oldY);

    bool capture = false;
    if (getPiece(square.x, square.y) != nullptr) {
        capture = true;
        deletePiece(square.x, square.y);
    }

    setPiece(std::move(piece), square);

    return capture;
}

void Board::invertPosition() {
    for (int j = 0, y = 7; j < 4; j++, y--) {
        for (int i = 0, x = 7; i < 8; i++, x--) {
            if (pieces[i][j] == nullptr && pieces[x][y] == nullptr)
                continue;

            std::swap(pieces[i][j], pieces[x][y]);

            if (pieces[i][j] != nullptr)
                pieces[i][j]->setSquare(Square{i, j, squareSize});
            if (pieces[x][y] != nullptr)
                pieces[x][y]->setSquare(Square{x, y, squareSize});
        }
    }

    orientation = !orientation;

    // Repaint move squares at their new flipped positions
    if (lastMoveFrom.x >= 0) {
        lastMoveFrom = {7 - lastMoveFrom.x, 7 - lastMoveFrom.y, squareSize};
        lastMoveTo = {7 - lastMoveTo.x, 7 - lastMoveTo.y, squareSize};

        moveSquare[0] = std::make_unique<sf::RectangleShape>(board[lastMoveFrom.x][lastMoveFrom.y]);
        moveSquare[1] = std::make_unique<sf::RectangleShape>(board[lastMoveTo.x][lastMoveTo.y]);

        moveSquare[0]->setFillColor(MOVECOLOR);
        moveSquare[1]->setFillColor(MOVECOLOR);
    }

    // Also update check highlight if present
    if (checkSquare != nullptr) {
        Square whiteKing = findKing(true);
        Square blackKing = findKing(false);
        bool whiteInCheck = isKingInCheck(true);
        bool blackInCheck = isKingInCheck(false);

        if (whiteInCheck) {
            checkSquare = std::make_unique<sf::RectangleShape>(board[whiteKing.x][whiteKing.y]);
            checkSquare->setFillColor(CHECKCOLOR);
        } else if (blackInCheck) {
            checkSquare = std::make_unique<sf::RectangleShape>(board[blackKing.x][blackKing.y]);
            checkSquare->setFillColor(CHECKCOLOR);
        }
    }
}

bool Board::getOrientation() { return orientation; }

void Board::paintMove(Square old, Square moved) {
    // Store logical positions for repaint after flip
    lastMoveFrom = old;
    lastMoveTo = moved;

    moveSquare[0] = std::make_unique<sf::RectangleShape>(board[old.x][old.y]);
    moveSquare[1] = std::make_unique<sf::RectangleShape>(board[moved.x][moved.y]);

    moveSquare[0]->setFillColor(MOVECOLOR);
    moveSquare[1]->setFillColor(MOVECOLOR);
}

void Board::resetColors() {
    moveSquare[0].reset();
    moveSquare[1].reset();
    lastMoveFrom = {-1, -1, 0};
    lastMoveTo = {-1, -1, 0};
}

bool Board::isPainted() { return moveSquare[0] != nullptr; }

Square Board::findKing(bool isWhite) {
    char kingType = isWhite ? 'K' : 'k';
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (pieces[i][j] != nullptr && pieces[i][j]->getType() == kingType) {
                return Square{i, j, squareSize};
            }
        }
    }
    return Square{-1, -1, squareSize};
}

bool Board::isSquareAttacked(Square target, bool byWhite) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::shared_ptr<Piece> piece = pieces[i][j];
            if (piece == nullptr || piece->isWhite() != byWhite)
                continue;

            char type = std::tolower(piece->getType());
            int dx = target.x - i;
            int dy = target.y - j;
            int absDx = abs(dx);
            int absDy = abs(dy);

            bool canAttack = false;

            switch (type) {
            case 'p': {
                int direction = byWhite ? -1 : 1;
                if (!orientation)
                    direction *= -1;
                if (absDx == 1 && dy == direction)
                    canAttack = true;
                break;
            }
            case 'n':
                if ((absDx == 2 && absDy == 1) || (absDx == 1 && absDy == 2))
                    canAttack = true;
                break;
            case 'k':
                if (absDx <= 1 && absDy <= 1 && (absDx + absDy > 0))
                    canAttack = true;
                break;
            case 'r':
                if ((dx == 0 || dy == 0) && (absDx + absDy > 0))
                    canAttack = true;
                break;
            case 'b':
                if (absDx == absDy && absDx > 0)
                    canAttack = true;
                break;
            case 'q':
                if ((dx == 0 || dy == 0 || absDx == absDy) && (absDx + absDy > 0))
                    canAttack = true;
                break;
            }

            if (!canAttack)
                continue;

            // For sliding pieces, check if path is clear
            if (type == 'r' || type == 'b' || type == 'q') {
                int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
                int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
                int x = i + stepX;
                int y = j + stepY;
                bool pathClear = true;

                while (x != target.x || y != target.y) {
                    if (pieces[x][y] != nullptr) {
                        pathClear = false;
                        break;
                    }
                    x += stepX;
                    y += stepY;
                }

                if (!pathClear)
                    continue;
            }

            return true;
        }
    }
    return false;
}

bool Board::isKingInCheck(bool isWhite) {
    Square kingPos = findKing(isWhite);
    if (kingPos.x == -1)
        return false;
    return isSquareAttacked(kingPos, !isWhite);
}

void Board::highlightCheck(bool isWhite, bool highlight) {
    if (highlight) {
        Square kingPos = findKing(isWhite);
        if (kingPos.x != -1) {
            checkSquare = std::make_unique<sf::RectangleShape>(board[kingPos.x][kingPos.y]);
            checkSquare->setFillColor(CHECKCOLOR);
        }
    } else {
        checkSquare.reset();
    }
}
