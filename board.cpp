#include "board.h"

Board::Board(float squareSize, std::string piecesSetup) : squareSize(squareSize) {
    orientation = true;
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            board[i][j].setSize({squareSize, squareSize});
            board[i][j].setPosition({i * squareSize, j * squareSize});

            sf::Color color = findColor(i, j);
            board[i][j].setFillColor(color);
        }
    }

    initialBoard(piecesSetup);
}

sf::Color Board::findColor(int i, int j) {
    return (i + j) % 2 == 0 ? whiteColor : blackColor;
}

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

Piece* Board::getPiece(int x, int y) {
    return pieces[x][y];
}

Piece* Board::createPiece(char piece, Square square) {
    switch (std::tolower(piece)) {
        case 'p':
            return new Pawn(piece, square);
        case 'r':
            return new Rook(piece, square);
        case 'n':
            return new Knight(piece, square);
        case 'b':
            return new Bishop(piece, square);
        case 'k':
            return new King(piece, square);
        case 'q':
            return new Queen(piece, square);
        default:
            std::cout << "Error!\n";
            return nullptr;
    }
}
void Board::setPiece(char pieceType, Square square) {
    pieces[square.x][square.y] = createPiece(pieceType, square);
}

void Board::setPiece(Piece* piece, Square square) {
    piece->setSquare(square);
    pieces[square.x][square.y] = piece;
}

void Board::deletePiece(int x, int y) {
    pieces[x][y] = nullptr;
}

void Board::movePiece(Piece* piece, Square square) {
    int oldX = piece->getSquare().x;
    int oldY = piece->getSquare().y;

    deletePiece(oldX, oldY);

    setPiece(piece, square);
}

void Board::invertPosition() {
    for (int j = 0, y = 7; j < 4; j++, y--) {
        for (int i = 0, x = 7; i < 8; i++, x--) {
            if (pieces[i][j] == nullptr && pieces[x][y] == nullptr)
                continue;

            swap(pieces[i][j], pieces[x][y]);

            if (pieces[i][j] != nullptr) pieces[i][j]->setSquare(Square{i, j, squareSize});
            if (pieces[x][y] != nullptr) pieces[x][y]->setSquare(Square{x, y, squareSize});
        }
    }

    orientation = !orientation;
}

template <typename T>
void Board::swap(T*& a, T*& b) {
    if (a == nullptr) {
        a = b;
        b = nullptr;
    } else if (b == nullptr) {
        b = a;
        a = nullptr;
    } else 
        std::swap(a, b);
}

bool Board::getOrientation() {
    return orientation;
}

void Board::paintMove(Square old, Square moved) {
    board[old.x][old.y].setFillColor(moveColor);
    board[moved.x][moved.y].setFillColor(moveColor);
}

void Board::resetColors() {
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            sf::Color color = findColor(i, j);
            board[i][j].setFillColor(color);
        }
    }
}
