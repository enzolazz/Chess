#pragma once

#include "Board.hpp"
#include "Piece.hpp"
#include "Sound.hpp"
#include <SFML/Graphics.hpp>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

typedef std::tuple<std::shared_ptr<Piece>, Square, std::shared_ptr<Piece>, Square, bool> move_tuple;
typedef std::vector<sf::CircleShape> circle_move;

class Game {
 private:
    sf::RenderWindow &_window;

    float squareSize;
    bool whiteTurn;
    int allowedToEnPassant;
    std::string initialBoard;

    Sound sounds;
    Board *board;

    std::shared_ptr<Piece> moving;
    std::shared_ptr<Piece> lastMovedPawn;
    std::stack<move_tuple> moves;
    std::stack<move_tuple> redoMoves;

    bool isLegalMove(std::shared_ptr<Piece> piece, Square *newSquare);
    bool turnCheck(char pieceType);
    void toggleTurn();
    char getPieceType(std::shared_ptr<Piece> piece);
    bool sameColorCapture(std::shared_ptr<Piece> moved, std::shared_ptr<Piece> taken);

 public:
    Game(sf::RenderWindow &window, float squareSize);

    void draw();
    bool isWhiteMove();
    bool piecePressed(const sf::Vector2i &position);
    bool pieceReleased(const sf::Vector2i &position);
    void pieceDrag();
    void resetMoving();
    void undo();
    void redo();
    void showAvailableSquares();
    bool pieceSelected();
    void switchSides();
};
