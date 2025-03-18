#include "Sound.h"
#include <SFML/Audio/SoundBuffer.hpp>
#include <iostream>

Sound::Sound() {
    if (!bufferMove.loadFromFile("etc/sounds/move.mp3")) {
        std::cerr << "Error loading move sound file" << std::endl;
    }

    if (!bufferCapture.loadFromFile("etc/sounds/capture.mp3")) {
        std::cerr << "Error loading capture sound file" << std::endl;
    }

    if (!bufferCheck.loadFromFile("etc/sounds/check.mp3")) {
        std::cerr << "Error loading check sound file" << std::endl;
    }
}

void Sound::playStart() {
    if (!buffer.loadFromFile("etc/sounds/game-start.mp3")) {
        std::cerr << "Error loading sound file" << std::endl;
    }

    sf::Sound sound(buffer);
    sound.play();
}

void Sound::playMove() {
    sf::Sound sound(bufferMove);
    sound.play();
}

void Sound::playCapture() {
    sf::Sound sound(bufferCapture);
    sound.play();
}
