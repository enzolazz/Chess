class Board {
public:
    sf::RectangleShape squares[8][8];
    sf::Color whiteColor = sf::Color(48, 97, 65/*, 38*/);
    sf::Color blackColor = sf::Color(36, 54, 43/*, 21*/);
    bool isWhiteMove = true;

    char pieces[8][8] = {
            {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
            {'0', '0', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '0', '0', '0'},
            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
            {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };

    Board(float squareSize) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                squares[i][j].setSize({squareSize, squareSize});
                squares[i][j].setPosition({i * squareSize, j * squareSize});

                sf::Color color = (i + j) % 2 == 0 ? whiteColor : blackColor;
                squares[i][j].setFillColor(color);
            }
        }
    }
};
