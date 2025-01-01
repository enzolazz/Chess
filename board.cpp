#include "board.h"

Board::Board(float squareSize, std::string piecesSetup) : _squareSize(squareSize) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j].setSize({squareSize, squareSize});
            board[i][j].setPosition({i * squareSize, j * squareSize});

            sf::Color color = (i + j) % 2 == 0 ? whiteColor : blackColor;
            board[i][j].setFillColor(color);
        }
    }

    initialBoard(piecesSetup);
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
                setPiece(i, j, piece);
        }
    }
}

Piece* Board::getPiece(int x, int y) {
    return pieces[x][y];
}

void Board::setPiece(int x, int y, char piece) {
    Square square = {x, y, _squareSize};
    pieces[x][y] = new Piece(piece, Square{x, y, _squareSize});
}

void Board::setPiece(int x, int y, Piece* piece) {
    //if (pieces[x][y] != nullptr) delete pieces[x][y];

    piece->setSquare(x, y);
    pieces[x][y] = piece;
}

void Board::deletePiece(int x, int y) {
    pieces[x][y] = nullptr;
    //delete pieces[x][y];
}

void Board::movePiece(Piece* piece, const sf::Vector2i &position) {
    int oldX = piece->getPosition().x / _squareSize;
    int oldY = piece->getPosition().y / _squareSize;

    //deletePiece(oldX, oldY);

    int x = position.x / _squareSize;
    int y = position.y / _squareSize;

    setPiece(x, y, piece);
}

void Board::invertPosition() {
    //std::vector<std::vector<Piece*>> pieces = std::vector<std::vector<Piece*>> (8, std::vector<Piece*>(8));
    for (int j = 0, y = 7; j < 4; j++, y--) {
        for (int i = 0, x = 7; i < 8; i++, x--) {
            if (pieces[i][j] == nullptr && pieces[x][y] == nullptr)
                continue;

            if (pieces[i][j] != nullptr) pieces[i][j]->setSquare(x, y);
            if (pieces[x][y] != nullptr) pieces[x][y]->setSquare(i, j);

            //swap(pieces[i][j], pieces[x][y]);
        }
    }
    std::cout << "Inverted!" << std::endl;
}

void Board::swap(Piece* a, Piece* b) {
    if (a == nullptr && b == nullptr) return;

    Piece* tmp = a;
    a = b;
    b = tmp;
}
