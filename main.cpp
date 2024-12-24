#include <SFML/Graphics.hpp>
#include "board.cpp"

const int squareSize = 5 * 25;

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
        board.draw(window);
        window.display();
    }
}
