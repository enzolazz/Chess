#include <SFML/Graphics.hpp>
#include "board.cpp"

const int squareSize = 5 * 25;

void drawBoard(Board &board, sf::RenderWindow &window) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            window.draw(board.squares[i][j]);
}

int main() {
    sf::RenderWindow window(sf::VideoMode({squareSize * 8, squareSize * 8}), "Chess-app");
    sf::RectangleShape whiteSquare({squareSize, squareSize});
    sf::RectangleShape blackSquare({squareSize, squareSize});

    whiteSquare.setFillColor(sf::Color(48, 97, 65, 38));
    blackSquare.setFillColor(sf::Color(36, 54, 43, 21));

    Board board(squareSize);
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        drawBoard(board, window);
        window.display();
    }
}
