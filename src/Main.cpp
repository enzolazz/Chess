#include "Game.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

const int squareSize = 110;

int main() {
    sf::RenderWindow window(sf::VideoMode({squareSize * 8, squareSize * 8}), "Chess-app");

    Game game(window, squareSize);

    bool M1Hold = false;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto *mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                switch (mouse->button) {
                case sf::Mouse::Button::Left: {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (game.piecePressed(mousePos))
                        M1Hold = true;
                    else
                        game.pieceReleased(mousePos);
                    break;
                }
                case sf::Mouse::Button::Right:
                    game.resetMoving();
                    M1Hold = false;
                    break;
                default:
                    break;
                }
            }

            if (const auto *mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (game.pieceSelected() && mouse->button == sf::Mouse::Button::Left) {
                    game.pieceReleased(sf::Mouse::getPosition(window));
                    M1Hold = false;
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

            if (M1Hold)
                game.pieceDrag();
        }

        window.clear();
        game.draw();
        window.display();
    }

    return 0;
}
