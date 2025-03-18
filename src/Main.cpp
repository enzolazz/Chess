#include "Game.h"
#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

const int squareSize = 110;

int main() {
    sf::RenderWindow window(sf::VideoMode({squareSize * 8, squareSize * 8}), "Chess-app");

    Game game(window, squareSize);

    bool moving = false;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            // Window closed or escape key pressed: exit
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto *mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                switch (mouse->button) {
                case sf::Mouse::Button::Left:
                    moving = game.piecePressed(sf::Mouse::getPosition(window));
                    break;
                case sf::Mouse::Button::Right:
                    game.pieceReleased();
                    moving = false;
                    break;
                default:
                    break;
                }
            }

            if (const auto *mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouse->button == sf::Mouse::Button::Left) {
                    game.pieceReleased();
                    moving = false;
                }
            }

            if (const auto *key = event->getIf<sf::Event::KeyPressed>()) {
                switch (key->code) {
                case sf::Keyboard::Key::R:
                    game.switchSides();
                    break;
                case sf::Keyboard::Key::Left:
                    game.undo();
                    break;
                case sf::Keyboard::Key::Right:
                    game.redo();
                    break;
                case sf::Keyboard::Key::Escape:
                    window.close();
                default:
                    break;
                }
            }

            if (moving)
                game.pieceDrag();
        }

        window.clear();
        game.draw();
        window.display();
    }

    return 0;
}
