#pragma once

#include <SFML/Audio.hpp>

class Sound {
 private:
    sf::SoundBuffer buffer;
    sf::SoundBuffer bufferMove, bufferCapture, bufferCheck;

 public:
    Sound();
    void playMove();
    void playCapture();
    void playCastle();
    void playCheck();
    void playCheckmate();
    void playIlegal();
    void playPromotion();
    void playStart();
    void playEnd();
};
