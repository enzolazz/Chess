#include "Sound.h"
#include <SFML/Audio/SoundSource.hpp>
#include <iostream>

Sound::Sound() {
    loadSound("etc/sounds/move.mp3");
    loadSound("etc/sounds/capture.mp3");
    loadSound("etc/sounds/check.mp3");
    loadSound("etc/sounds/illegal.mp3");
    loadSound("etc/sounds/game-start.mp3");
    loadSound("etc/sounds/game-end.mp3");
    loadSound("etc/sounds/promotion.mp3");
    loadSound("etc/sounds/castle.mp3");
}

void Sound::loadSound(const std::string &filename) {
    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(filename)) {
        std::cerr << "Error loading sound file: " << filename << std::endl;
    }
    buffers.push_back(std::move(buffer));

    auto sound = std::make_unique<sf::Sound>(*buffers.back());
    sounds.push_back(std::move(sound));
}

bool Sound::isAnySoundPlaying() const {
    for (const auto &sound : sounds) {
        if (sound->getStatus() == sf::SoundSource::Status::Playing) {
            return true;
        }
    }
    return false;
}

void Sound::playMove() {
    if (!isAnySoundPlaying())
        sounds[0]->play();
}

void Sound::playIlegal() {
    if (!isAnySoundPlaying())
        sounds[3]->play();
}

void Sound::playCapture() {
    if (!isAnySoundPlaying())
        sounds[1]->play();
}

void Sound::playStart() { sounds[4]->play(); }
void Sound::playCheck() { sounds[2]->play(); }
void Sound::playPromotion() { sounds[6]->play(); }
void Sound::playEnd() { sounds[5]->play(); }
void Sound::playCastle() { sounds[7]->play(); }
