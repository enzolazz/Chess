#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "game.cpp"
#include <iostream>

const int squareSize = 110;

int main() {
    sf::RenderWindow window(sf::VideoMode({squareSize * 8, squareSize * 8}), "Chess-app");
    bool moving = false;

    Game game(window, squareSize);
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            // Window closed or escape key pressed: exit
            if (event->is<sf::Event::Closed>() ||
                (event->is<sf::Event::KeyPressed>() &&
                event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>() &&
                event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                moving = game.piecePressed(sf::Mouse::getPosition(window));
            }

            if (event->is<sf::Event::MouseButtonReleased>() &&
                moving && event->getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Left) {
                game.pieceReleased();
                moving = false;
            }

            if (event->is<sf::Event::KeyPressed>() &&
                event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::R)
                game.switchSides();

            if (event->is<sf::Event::KeyPressed>() &&
                event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Left)
                game.undo();

            if (event->is<sf::Event::KeyPressed>() &&
                event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Right)
                game.redo();

            if (moving)
                game.pieceDrag();
        }

        window.clear();
        game.draw();
        window.display();
    }

    return 0;
}
