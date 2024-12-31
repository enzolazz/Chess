#include "piece.h"
#include <vector>

class Board {
private:
    sf::Color whiteColor = sf::Color(240, 217, 181); // Branco clássico
    sf::Color blackColor = sf::Color(181, 136, 99);  // Marrom clássico
    sf::RectangleShape board[8][8];

    int _squareSize;
    std::vector<std::vector<Piece*>> pieces;

    void initialBoard(std::string setup);
    void setPiece(int x, int y, char pieceType);
    void setPiece(int x, int y, Piece *piece);
    void deletePìece(int x, int y);
    void movePiece(Piece* piece, const sf::Vector2i &position);

public:
    Board(float squareSize, std::string piecesSetup);
    Piece* getPiece(int x, int y); 
    void setColor(sf::Color newColor);
};
