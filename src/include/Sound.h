#pragma once

#include <SFML/Audio.hpp>
#include <memory>
#include <vector>

class Sound {
 private:
    std::vector<std::unique_ptr<sf::SoundBuffer>> buffers;
    std::vector<std::unique_ptr<sf::Sound>> sounds;

    void loadSound(const std::string &filename);

 public:
    Sound();
    void playMove();
    void playCapture();
    void playCheck();
    void playIlegal();
    void playPromotion();
    void playStart();
    void playEnd();
    void playCastle();
};
