#include "piece.h"
#include <vector>
#include <SFML/Graphics.hpp>

class Board {
private:
    bool _isWhiteMove = true;
    std::string _initialBoard = "rnbqkbnrpppppppp8888PPPPPPPPRNBQKBNR";
    sf::Color whiteColor = sf::Color(240, 217, 181); // Branco clássico
    sf::Color blackColor = sf::Color(181, 136, 99);  // Marrom clássico

    float _squareSize;
    std::vector<std::vector<Piece>> pieces;

    void updatePieces();
    void setPiece(int x, int y, char piece);

public:
    sf::RectangleShape squares[8][8];

    Board(float squareSize);
    void draw(sf::RenderWindow &window);
    bool isWhiteMove() const { return _isWhiteMove; }
    void toggleTurn() { _isWhiteMove = !_isWhiteMove; }

    const Piece& getPiece(int x, int y) const { return pieces[x][y]; }
};

Board::Board(float squareSize) : _squareSize(squareSize) {
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
    pieces[x][y] = Piece(piece, {x * _squareSize, y * _squareSize});
}

void Board::draw(sf::RenderWindow &window) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            window.draw(squares[i][j]);
            if (pieces[i][j].piece != '0') { // Verifica se a peça não é vazia
                window.draw(pieces[i][j].getSprite()); // Usa referência, não ponteiro
            }
        }
    }
}


