#pragma once

#include "Board.h"
#include "Piece.h"
#include <SFML/Graphics.hpp>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

typedef std::tuple<Piece *, Square, Piece *, Square, bool> move_tuple;
typedef std::vector<sf::CircleShape> circle_move;

class Game {
 private:
    sf::RenderWindow &_window;
    float squareSize;
    bool whiteTurn = true;
    std::string initialBoard = "rnbqkbnrpppppppp8888PPPPPPPPRNBQKBNR";
    Board *board;
    Piece *moving = nullptr;
    std::stack<move_tuple> moves;
    std::stack<move_tuple> redoMoves;

    bool isLegalMove(Piece *piece, Square *newSquare);
    bool turnCheck(char pieceType);
    void toggleTurn();
    char getPieceType(Piece *piece);
    bool sameColorCapture(Piece *moved, Piece *taken);

 public:
    Game(sf::RenderWindow &window, float squareSize);

    void draw();
    bool isWhiteMove();
    bool piecePressed(const sf::Vector2i &position);
    void pieceReleased();
    void pieceDrag();
    void undo();
    void redo();
    void showAvailableSquares();
    bool pieceSelected();
    void switchSides();
};
