#include <SFML/Graphics.hpp>
#include "board.cpp"
#include <iostream>

const int squareSize = 110;

int main() {
    sf::RenderWindow window(sf::VideoMode({squareSize * 8, squareSize * 8}), "Chess-app");
    sf::RectangleShape whiteSquare({squareSize, squareSize});
    sf::RectangleShape blackSquare({squareSize, squareSize});

    whiteSquare.setFillColor(sf::Color(48, 97, 65, 38));
    blackSquare.setFillColor(sf::Color(36, 54, 43, 21));

    bool moving = false, legalMove = false;

    Board board(window, squareSize);
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (event->is<sf::Event::MouseButtonPressed>()) {
                moving = board.piecePressed(sf::Mouse::getPosition(window));
            }
            else if (event->is<sf::Event::MouseButtonReleased>() && moving) {
                board.pieceReleased();
                moving = false;
            }

            if (moving) {
                board.pieceDrag();
            }
        }

        window.clear();
        board.draw(window);
        window.display();
    }
}
