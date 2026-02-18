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

enum class GameState { PLAYING, PROMOTING, CHECKMATE, STALEMATE };

class Game {
 private:
    sf::RenderWindow &_window;

    float squareSize;
    bool whiteTurn;
    int allowedToEnPassant;
    std::string initialBoard;
    GameState gameState;

    Sound sounds;
    std::unique_ptr<Board> board;
    std::unique_ptr<sf::Font> font;
    std::unique_ptr<sf::Text> endGameText;
    bool fontLoaded;

    std::shared_ptr<Piece> moving;
    std::shared_ptr<Piece> lastMovedPawn;
    std::stack<move_tuple> moves;
    std::stack<move_tuple> redoMoves;

    // Promotion state
    std::shared_ptr<Piece> promotingPawn;
    Square promotionSquare;
    std::shared_ptr<Piece> promotionCaptured;

    bool isLegalMove(std::shared_ptr<Piece> piece, Square *newSquare);
    bool turnCheck(char pieceType);
    void toggleTurn();
    char getPieceType(std::shared_ptr<Piece> piece);
    bool sameColorCapture(std::shared_ptr<Piece> moved, std::shared_ptr<Piece> taken);
    bool wouldLeaveKingInCheck(std::shared_ptr<Piece> piece, Square newSquare);
    bool isSquareAttackedForCastling(Square target, bool byWhite);
    bool hasLegalMoves(bool isWhite);
    GameState checkGameState();
    void drawEndGame();
    void drawPromotionUI();
    bool handlePromotionClick(const sf::Vector2i &position);
    void completePromotion(char pieceType);
    bool isPawnPromotion(std::shared_ptr<Piece> piece, Square targetSquare);

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
    void restart();
    bool isGameOver();
};
