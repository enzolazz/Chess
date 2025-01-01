#include "piece.h"
#include <vector>

class Board {
private:
    sf::Color whiteColor = sf::Color(240, 217, 181); // Branco clássico
    sf::Color blackColor = sf::Color(181, 136, 99);  // Marrom clássico

    float _squareSize;
    std::vector<std::vector<Piece*>> pieces = std::vector<std::vector<Piece*>> (8, std::vector<Piece*>(8));

    void initialBoard(std::string setup);
    void setPiece(int x, int y, char pieceType);
    void setPiece(int x, int y, Piece *piece);
    void swap(Piece* a, Piece* b);
    void deletePiece(int x, int y);
public:
    sf::RectangleShape board[8][8];

    Board(float squareSize, std::string piecesSetup);
    Piece* getPiece(int x, int y); 
    void movePiece(Piece* piece, const sf::Vector2i &position);
    void invertPosition();
    void setColor(sf::Color newColor);
};
