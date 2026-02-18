#include "Game.hpp"
#include "Pawn.hpp"
#include "Rook.hpp"
#include <SFML/System/Vector2.hpp>
#include <memory>

static Square getSquare(const sf::Vector2i &position, float squareSize) {
    return {(int)(position.x / squareSize), (int)(position.y / squareSize), squareSize};
}

static bool sameSquare(Square &a, Square &b) { return a.x == b.x && a.y == b.y; }

Game::Game(sf::RenderWindow &window, float squareSize)
    : _window(window), squareSize(squareSize), whiteTurn(true), allowedToEnPassant(0),
      initialBoard("rnbqkbnrpppppppp8888PPPPPPPPRNBQKBNR"), gameState(GameState::PLAYING), fontLoaded(false) {
    board = std::make_unique<Board>(squareSize, initialBoard);

    font = std::make_unique<sf::Font>();
    fontLoaded = font->openFromFile("etc/fonts/Roboto-Bold.ttf");
    if (!fontLoaded) {
        fontLoaded = font->openFromFile("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf");
    }
    if (!fontLoaded) {
        fontLoaded = font->openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");
    }

    if (fontLoaded) {
        endGameText = std::make_unique<sf::Text>(*font, "", 48);
        endGameText->setFillColor(sf::Color::White);
        endGameText->setOutlineColor(sf::Color::Black);
        endGameText->setOutlineThickness(3.f);
    }

    sounds.playStart();
}

void Game::draw() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            _window.draw(board->board[i][j]);

    if (board->isPainted()) {
        _window.draw(*board->moveSquare[0]);
        _window.draw(*board->moveSquare[1]);
    }

    if (board->checkSquare != nullptr) {
        _window.draw(*board->checkSquare);
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::shared_ptr<Piece> piece = board->getPiece(i, j);
            if (piece != nullptr && piece != moving)
                _window.draw(piece->getSprite());
        }
    }

    if (moving != nullptr) {
        showAvailableSquares();
        _window.draw(moving->getSprite());
    }

    drawPromotionUI();
    drawEndGame();
}

bool Game::piecePressed(const sf::Vector2i &position) {
    if (gameState == GameState::PROMOTING) {
        handlePromotionClick(position);
        return false;
    }

    if (gameState != GameState::PLAYING)
        return false;

    int x = position.x / squareSize;
    int y = position.y / squareSize;

    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;

    std::shared_ptr<Piece> piece = board->getPiece(x, y);
    if (piece == nullptr || !turnCheck(piece->getType()))
        return false;

    board->resetColors();
    moving = piece;

    return true;
}

void Game::pieceDrag() {
    if (moving == nullptr)
        return;

    moving->drag(static_cast<sf::Vector2f>(sf::Mouse::getPosition(_window)));
}

bool Game::pieceReleased(const sf::Vector2i &position) {
    if (moving == nullptr)
        return false;

    int x = position.x / squareSize;
    int y = position.y / squareSize;

    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        resetMoving();
        return false;
    }

    Square pieceSquare = moving->getSquare();

    Square movingSquare = getSquare(position, squareSize);

    size_t length = moves.size();
    if (!isLegalMove(moving, &movingSquare)) {
        resetMoving();
        if (!sameSquare(pieceSquare, movingSquare))
            sounds.playIlegal();
        return false;
    }
    move_tuple move(moving, pieceSquare, board->getPiece(movingSquare.x, movingSquare.y), movingSquare,
                    board->getOrientation());
    moves.push(move);
    redoMoves = std::stack<move_tuple>();

    board->resetColors();
    board->paintMove(pieceSquare, movingSquare);

    // Check for pawn promotion before completing the move
    if (isPawnPromotion(moving, movingSquare)) {
        promotingPawn = moving;
        promotionSquare = movingSquare;
        promotionCaptured = board->getPiece(movingSquare.x, movingSquare.y);

        board->movePiece(moving, movingSquare);
        gameState = GameState::PROMOTING;
        moving = nullptr;
        return true;
    }

    bool captured = board->movePiece(moving, movingSquare);

    // Check if opponent's king is now in check
    bool opponentInCheck = board->isKingInCheck(!moving->isWhite());
    board->highlightCheck(!moving->isWhite(), opponentInCheck);

    if (opponentInCheck) {
        sounds.playCheck();
    } else if (length != moves.size() - 1 && !captured) {
        sounds.playCastle();
    } else if (captured) {
        sounds.playCapture();
    } else {
        sounds.playMove();
    }

    moving = nullptr;
    toggleTurn();

    // Check for checkmate/stalemate
    gameState = checkGameState();
    if (gameState != GameState::PLAYING) {
        sounds.playEnd();
    }

    return true;
}

void Game::resetMoving() {
    if (moving == nullptr)
        return;

    board->movePiece(moving, moving->getSquare());
    moving = nullptr;
}

bool Game::isLegalMove(std::shared_ptr<Piece> piece, Square *square) {
    int x = square->x;
    int y = square->y;

    int pieceX = piece->getSquare().x;
    int pieceY = piece->getSquare().y;

    if (x == pieceX && y == pieceY)
        return false;

    bool isValid;
    char pieceType = getPieceType(piece);
    int dX = pieceX - x;
    int dY = pieceY - y;

    std::shared_ptr<Rook> rook;
    std::shared_ptr<Piece> newSquarePiece = board->getPiece(x, y);

    isValid = piece->isValidMove(*square);

    switch (pieceType) {
    case 'p': {
        std::shared_ptr<Pawn> pawn = std::static_pointer_cast<Pawn>(piece);
        isValid = pawn->isValidMove(*square, board->getOrientation());

        if (!isValid)
            return false;

        if (abs(dX) == 1 && abs(dY) == 1 && newSquarePiece == nullptr) {
            std::shared_ptr<Piece> enPassantPiece = board->getPiece(x, pieceY);

            // En passant requires: valid timing, enemy pawn adjacent
            if (allowedToEnPassant != 0 || enPassantPiece == nullptr ||
                enPassantPiece->isWhite() == piece->isWhite() ||
                getPieceType(enPassantPiece) != 'p')
                return false;

            moves.push(
                move_tuple(enPassantPiece, enPassantPiece->getSquare(), nullptr, *square, board->getOrientation()));

            board->movePiece(enPassantPiece, *square);
        } else if (dX != 0 &&
                   (abs(dY) != 1 || newSquarePiece == nullptr || newSquarePiece->isWhite() == piece->isWhite())) {
            return false;
        } else if (dX == 0) {
            int orientationDir = board->getOrientation() ? 1 : -1;

            int i = pieceY;
            do {
                (piece->isWhite()) ? i -= 1 *orientationDir : i += 1 * orientationDir;
                if (board->getPiece(x, i) != nullptr)
                    return false;
            } while (i != y);
        }

        if (abs(dY) == 2) {
            lastMovedPawn = piece;
            allowedToEnPassant = -1;
        }
        break;
    }
    case 'k':
        if (abs(dX) >= 2) {
            int x = (dX > 0) ? 0 : 7;
            int y = piece->isWhite() ? 7 : 0;

            rook = std::static_pointer_cast<Rook>(board->getPiece(x, y));
        }

        if (!isValid)
            return false;

        // Will castle
        if (rook != nullptr) {
            // Can't castle if king is in check
            if (board->isKingInCheck(piece->isWhite()))
                return false;

            if (rook->hasMoved())
                return false;
            int rX = rook->getSquare().x, rY = rook->getSquare().y;

            int castleRook = -2;
            dX = 2;

            if (rook->getSquare().x == 0) {
                castleRook = 3;
                dX = -2;
            }

            Square castledSquare{rX + castleRook, rY, squareSize};

            {
                int i = pieceX;

                do {
                    (dX > 0) ? i++ : i--;

                    newSquarePiece = board->getPiece(i, pieceY);

                    if (sameColorCapture(piece, newSquarePiece))
                        return false;

                    // King can't pass through attacked square
                    if (i != (pieceX + dX) || i == pieceX + (dX > 0 ? 1 : -1)) {
                        Square passingSquare{i, pieceY, squareSize};
                        if (board->isSquareAttacked(passingSquare, !piece->isWhite()))
                            return false;
                    }
                } while (i != (pieceX + dX));
            }

            // Check that the final square is not attacked
            Square finalSquare{pieceX + dX, pieceY, squareSize};
            if (board->isSquareAttacked(finalSquare, !piece->isWhite()))
                return false;

            move_tuple rookCastled(rook, rook->getSquare(), nullptr, castledSquare, board->getOrientation());
            moves.push(rookCastled);

            board->movePiece(rook, castledSquare);
            rook->moved();

            square->x = pieceX + dX;
        } else {
            if (sameColorCapture(piece, newSquarePiece))
                return false;
        }
        break;
    case 'n':
        if (!isValid)
            return false;

        if (sameColorCapture(piece, newSquarePiece))
            return false;
        break;
    default:
        if (!isValid)
            return false;

        {
            int i = pieceX, j = pieceY;

            do {
                if (dX != 0)
                    (dX < 0) ? i++ : i--;
                if (dY != 0)
                    (dY < 0) ? j++ : j--;

                newSquarePiece = board->getPiece(i, j);

                if (sameColorCapture(piece, newSquarePiece))
                    return false;
            } while (i != x || j != y);
        }
        break;
    }

    // Check if this move would leave the king in check
    // For castling, we already validated attack checks on the path
    if (pieceType != 'k' || rook == nullptr) {
        if (wouldLeaveKingInCheck(piece, *square))
            return false;
    }

    allowedToEnPassant++;
    piece->moved();

    return true;
}

bool Game::turnCheck(char pieceType) {
    return (whiteTurn && std::isupper(pieceType)) || (!whiteTurn && std::islower(pieceType));
}

void Game::undo() {
    if (moves.empty())
        return;

    allowedToEnPassant--;
    move_tuple lastMove(moves.top());
    moves.pop();

    std::shared_ptr<Piece> movedPiece = std::get<0>(lastMove);
    std::shared_ptr<Piece> takenPiece = std::get<2>(lastMove);

    Square oldSquare = std::get<1>(lastMove);
    Square newSquare = std::get<3>(lastMove);

    board->resetColors();

    bool sameOrientation = std::get<4>(lastMove) != board->getOrientation();
    if (sameOrientation)
        board->invertPosition();

    board->movePiece(movedPiece, oldSquare);
    if (takenPiece != nullptr)
        board->movePiece(takenPiece, newSquare);

    movedPiece->unmoved();

    if (sameOrientation)
        board->invertPosition();

    char pieceType = getPieceType(movedPiece);
    if (pieceType == 'k' && abs(oldSquare.x - newSquare.x) == 2) {
        sounds.playCastle();
        undo();
    } else if (pieceType == 'p' && !moves.empty()) {
        if (abs(oldSquare.y - newSquare.y) == 2) {
            lastMovedPawn = movedPiece;
            allowedToEnPassant = 0;
        }

        move_tuple enPassantMove(moves.top());

        std::shared_ptr<Piece> lastPiece = std::get<0>(enPassantMove);
        Square oldSquare = std::get<1>(enPassantMove);
        Square newSquare = std::get<3>(enPassantMove);

        short direction = (lastPiece->isWhite() ? -1 : 1);
        if (getPieceType(lastPiece) == 'p' && (oldSquare.y - newSquare.y == 1 * direction)) {
            sounds.playCapture();
            undo();
            allowedToEnPassant = 0;
        } else {
            toggleTurn();
        }
    } else
        toggleTurn();

    if (takenPiece != nullptr)
        sounds.playCapture();
    else
        sounds.playMove();
    redoMoves.push(lastMove);

    // Update check highlight after undo
    bool whiteInCheck = board->isKingInCheck(true);
    bool blackInCheck = board->isKingInCheck(false);
    board->highlightCheck(true, whiteInCheck);
    if (!whiteInCheck)
        board->highlightCheck(false, blackInCheck);
}

void Game::redo() {
    if (redoMoves.empty())
        return;

    allowedToEnPassant++;
    move_tuple undoed(redoMoves.top());
    redoMoves.pop();

    std::shared_ptr<Piece> movedPiece = std::get<0>(undoed);
    Square oldSquare = std::get<1>(undoed);
    Square newSquare = std::get<3>(undoed);

    board->resetColors();
    board->paintMove(oldSquare, newSquare);

    bool sameOrientation = std::get<4>(undoed) != board->getOrientation();
    if (sameOrientation)
        board->invertPosition();

    board->movePiece(movedPiece, newSquare);
    movedPiece->moved();

    if (sameOrientation)
        board->invertPosition();

    char pieceType = getPieceType(movedPiece);
    if (pieceType == 'k' && abs(oldSquare.x - newSquare.x) == 2) {
        sounds.playCastle();
        redo();
    } else if (pieceType == 'p' && !redoMoves.empty()) {
        if (abs(oldSquare.y - newSquare.y) == 2) {
            lastMovedPawn = movedPiece;
            allowedToEnPassant = 0;
        }

        move_tuple enPassantMove(redoMoves.top());

        std::shared_ptr<Piece> lastPiece = std::get<0>(enPassantMove);
        Square oldSquare = std::get<1>(enPassantMove);
        Square newSquare = std::get<3>(enPassantMove);

        short direction = (lastPiece->isWhite() ? -1 : 1);
        if (getPieceType(lastPiece) == 'p' && (oldSquare.y - newSquare.y == 1 * direction)) {
            sounds.playCapture();
            redo();

            board->movePiece(movedPiece, newSquare);
        } else {
            toggleTurn();
        }
    } else
        toggleTurn();

    if (std::get<2>(undoed) != nullptr)
        sounds.playCapture();
    else
        sounds.playMove();
    moves.push(undoed);

    // Update check highlight after redo
    bool whiteInCheck = board->isKingInCheck(true);
    bool blackInCheck = board->isKingInCheck(false);
    board->highlightCheck(true, whiteInCheck);
    if (!whiteInCheck)
        board->highlightCheck(false, blackInCheck);
}

void Game::toggleTurn() {
    whiteTurn = !whiteTurn;
    // switchSides();
}

void Game::switchSides() { board->invertPosition(); }

void Game::showAvailableSquares() {
    square_list potentialMoves = moving->getMoves();
    circle_move circles;

    sf::Color moveColor = sf::Color(146, 146, 146, 146);
    sf::Color captureColor = sf::Color(200, 80, 80, 180);

    for (Square &square : potentialMoves) {
        // Skip out-of-bounds moves
        if (square.x < 0 || square.x >= 8 || square.y < 0 || square.y >= 8)
            continue;

        // Check if move would leave king in check
        if (wouldLeaveKingInCheck(moving, square))
            continue;

        // Check for same-color piece blocking
        std::shared_ptr<Piece> targetPiece = board->getPiece(square.x, square.y);
        if (targetPiece != nullptr && targetPiece->isWhite() == moving->isWhite())
            continue;

        // For sliding pieces, check if path is blocked
        char pieceType = getPieceType(moving);
        if (pieceType == 'r' || pieceType == 'b' || pieceType == 'q') {
            Square current = moving->getSquare();
            int dx = square.x - current.x;
            int dy = square.y - current.y;
            int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
            int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

            bool blocked = false;
            int x = current.x + stepX;
            int y = current.y + stepY;
            while (x != square.x || y != square.y) {
                if (board->getPiece(x, y) != nullptr) {
                    blocked = true;
                    break;
                }
                x += stepX;
                y += stepY;
            }
            if (blocked)
                continue;
        }

        // For pawns, use isValidMove which handles orientation correctly
        if (pieceType == 'p') {
            Square current = moving->getSquare();
            int dx = abs(square.x - current.x);

            // Use the pawn's isValidMove for direction validation
            std::shared_ptr<Pawn> pawn = std::static_pointer_cast<Pawn>(moving);
            if (!pawn->isValidMove(square, board->getOrientation()))
                continue;

            // Forward moves (dx == 0) require empty squares
            if (dx == 0) {
                if (targetPiece != nullptr)
                    continue;
                // For double move, check middle square
                int dy = square.y - current.y;
                if (abs(dy) == 2) {
                    int midY = current.y + (dy > 0 ? 1 : -1);
                    if (board->getPiece(square.x, midY) != nullptr)
                        continue;
                }
            }
            // Diagonal moves (dx == 1) require enemy capture or valid en passant
            else if (dx == 1) {
                if (targetPiece != nullptr) {
                    // Must be enemy piece to capture
                    if (targetPiece->isWhite() == moving->isWhite())
                        continue;
                } else {
                    // En passant: must be adjacent enemy pawn that just double-moved
                    std::shared_ptr<Piece> adjacentPiece = board->getPiece(square.x, current.y);
                    bool validEnPassant = (adjacentPiece != nullptr &&
                                          getPieceType(adjacentPiece) == 'p' &&
                                          adjacentPiece->isWhite() != moving->isWhite() &&
                                          allowedToEnPassant == 0);
                    if (!validEnPassant)
                        continue;
                }
            }
        }

        // Draw the move indicator
        bool isCapture = targetPiece != nullptr;
        sf::CircleShape circle(isCapture ? squareSize / 2 - 5.f : 12.f);

        if (isCapture) {
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(captureColor);
            circle.setOutlineThickness(5.f);
        } else {
            circle.setFillColor(moveColor);
        }

        circle.setOrigin({circle.getRadius(), circle.getRadius()});
        sf::Vector2f pos(square.x * squareSize + squareSize / 2, square.y * squareSize + squareSize / 2);
        circle.setPosition({pos.x, pos.y});

        circles.push_back(circle);
    }

    for (auto &shape : circles) {
        _window.draw(shape);
    }
}

bool Game::pieceSelected() { return moving != nullptr; }

char Game::getPieceType(std::shared_ptr<Piece> piece) { return std::tolower(piece->getType()); }

bool Game::sameColorCapture(std::shared_ptr<Piece> moved, std::shared_ptr<Piece> taken) {
    if (taken == nullptr)
        return false;

    if (moved->isWhite() != taken->isWhite())
        return false;

    return true;
}

bool Game::wouldLeaveKingInCheck(std::shared_ptr<Piece> piece, Square newSquare) {
    Square oldSquare = piece->getSquare();
    std::shared_ptr<Piece> capturedPiece = board->getPiece(newSquare.x, newSquare.y);

    // Save sprite position before simulation (for dragging)
    sf::Vector2f savedSpritePos = piece->getSprite().getPosition();

    // Simulate the move (only board state, not visual)
    board->getPiece(oldSquare.x, oldSquare.y) = nullptr;
    board->getPiece(newSquare.x, newSquare.y) = piece;
    Square savedSquare = piece->getSquare();
    piece->setSquare(newSquare);

    // Check if own king is in check
    bool inCheck = board->isKingInCheck(piece->isWhite());

    // Undo the simulation
    board->getPiece(oldSquare.x, oldSquare.y) = piece;
    board->getPiece(newSquare.x, newSquare.y) = capturedPiece;
    piece->setSquare(savedSquare);

    // Restore sprite position for dragging
    piece->getSprite().setPosition(savedSpritePos);

    return inCheck;
}

bool Game::isSquareAttackedForCastling(Square target, bool byWhite) {
    return board->isSquareAttacked(target, byWhite);
}

bool Game::hasLegalMoves(bool isWhite) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::shared_ptr<Piece> piece = board->getPiece(i, j);
            if (piece == nullptr || piece->isWhite() != isWhite)
                continue;

            // Try all possible destination squares
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    if (x == i && y == j)
                        continue;

                    Square targetSquare{x, y, squareSize};

                    // Check basic move validity
                    if (!piece->isValidMove(targetSquare))
                        continue;

                    // Check if move would leave king in check
                    if (!wouldLeaveKingInCheck(piece, targetSquare)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

GameState Game::checkGameState() {
    bool currentPlayerIsWhite = whiteTurn;
    bool inCheck = board->isKingInCheck(currentPlayerIsWhite);
    bool hasLegal = hasLegalMoves(currentPlayerIsWhite);

    if (!hasLegal) {
        if (inCheck) {
            return GameState::CHECKMATE;
        } else {
            return GameState::STALEMATE;
        }
    }

    return GameState::PLAYING;
}

void Game::drawEndGame() {
    if (gameState == GameState::PLAYING)
        return;

    // Draw semi-transparent overlay
    sf::RectangleShape overlay({squareSize * 8, squareSize * 8});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    _window.draw(overlay);

    if (!fontLoaded)
        return;

    // Draw end game message
    std::string message;
    if (gameState == GameState::CHECKMATE) {
        message = whiteTurn ? "Black wins!" : "White wins!";
    } else {
        message = "Stalemate - Draw!";
    }

    endGameText->setString(message);
    sf::FloatRect textBounds = endGameText->getLocalBounds();
    endGameText->setOrigin({textBounds.position.x + textBounds.size.x / 2.f,
                            textBounds.position.y + textBounds.size.y / 2.f});
    endGameText->setPosition({squareSize * 4, squareSize * 3.5f});
    _window.draw(*endGameText);

    // Draw restart instruction
    sf::Text restartText(*font, "Press N to start new game", 24);
    restartText.setFillColor(sf::Color::White);
    restartText.setOutlineColor(sf::Color::Black);
    restartText.setOutlineThickness(2.f);
    sf::FloatRect restartBounds = restartText.getLocalBounds();
    restartText.setOrigin({restartBounds.position.x + restartBounds.size.x / 2.f,
                           restartBounds.position.y + restartBounds.size.y / 2.f});
    restartText.setPosition({squareSize * 4, squareSize * 4.5f});
    _window.draw(restartText);
}

void Game::restart() {
    board = std::make_unique<Board>(squareSize, initialBoard);
    whiteTurn = true;
    allowedToEnPassant = 0;
    gameState = GameState::PLAYING;
    moving = nullptr;
    lastMovedPawn = nullptr;
    moves = std::stack<move_tuple>();
    redoMoves = std::stack<move_tuple>();
    sounds.playStart();
}

bool Game::isGameOver() { return gameState != GameState::PLAYING && gameState != GameState::PROMOTING; }

bool Game::isPawnPromotion(std::shared_ptr<Piece> piece, Square targetSquare) {
    if (getPieceType(piece) != 'p')
        return false;

    int lastRank = piece->isWhite() ? 0 : 7;
    if (!board->getOrientation())
        lastRank = 7 - lastRank;

    return targetSquare.y == lastRank;
}

void Game::drawPromotionUI() {
    if (gameState != GameState::PROMOTING)
        return;

    // Determine UI position based on promotion square
    int uiX = promotionSquare.x;
    int uiStartY = promotingPawn->isWhite() ? 0 : 4;
    if (!board->getOrientation())
        uiStartY = 7 - uiStartY - 3;

    // Draw background overlay
    sf::RectangleShape overlay({squareSize * 8, squareSize * 8});
    overlay.setFillColor(sf::Color(0, 0, 0, 100));
    _window.draw(overlay);

    // Draw promotion panel
    sf::RectangleShape panel({squareSize, squareSize * 4});
    panel.setPosition({uiX * squareSize, uiStartY * squareSize});
    panel.setFillColor(sf::Color(240, 240, 240));
    panel.setOutlineColor(sf::Color::Black);
    panel.setOutlineThickness(2.f);
    _window.draw(panel);

    // Piece options: Queen, Rook, Bishop, Knight
    char pieceTypes[] = {'q', 'r', 'b', 'n'};
    if (promotingPawn->isWhite()) {
        pieceTypes[0] = 'Q';
        pieceTypes[1] = 'R';
        pieceTypes[2] = 'B';
        pieceTypes[3] = 'N';
    }

    for (int i = 0; i < 4; i++) {
        int y = uiStartY + i;
        if (!promotingPawn->isWhite() && board->getOrientation())
            y = uiStartY + (3 - i);
        else if (promotingPawn->isWhite() && !board->getOrientation())
            y = uiStartY + (3 - i);

        Square optionSquare{uiX, y, squareSize};
        auto tempPiece = board->createPiece(pieceTypes[i], optionSquare);
        if (tempPiece)
            _window.draw(tempPiece->getSprite());
    }
}

bool Game::handlePromotionClick(const sf::Vector2i &position) {
    if (gameState != GameState::PROMOTING)
        return false;

    int clickX = position.x / squareSize;
    int clickY = position.y / squareSize;

    int uiX = promotionSquare.x;
    int uiStartY = promotingPawn->isWhite() ? 0 : 4;
    if (!board->getOrientation())
        uiStartY = 7 - uiStartY - 3;

    if (clickX != uiX)
        return false;

    if (clickY < uiStartY || clickY >= uiStartY + 4)
        return false;

    int selection = clickY - uiStartY;
    if (!promotingPawn->isWhite() && board->getOrientation())
        selection = 3 - selection;
    else if (promotingPawn->isWhite() && !board->getOrientation())
        selection = 3 - selection;

    char pieceTypes[] = {'q', 'r', 'b', 'n'};
    if (promotingPawn->isWhite()) {
        pieceTypes[0] = 'Q';
        pieceTypes[1] = 'R';
        pieceTypes[2] = 'B';
        pieceTypes[3] = 'N';
    }

    completePromotion(pieceTypes[selection]);
    return true;
}

void Game::completePromotion(char pieceType) {
    // Create the promoted piece
    std::shared_ptr<Piece> promotedPiece = board->createPiece(pieceType, promotionSquare);

    // Place it on the board
    board->getPiece(promotionSquare.x, promotionSquare.y) = promotedPiece;

    // Record the move for undo (the pawn move is already recorded, just update)
    // The move is already pushed in pieceReleased, so we're good

    sounds.playPromotion();

    gameState = GameState::PLAYING;
    toggleTurn();

    // Check for checkmate/stalemate after promotion
    bool opponentInCheck = board->isKingInCheck(whiteTurn);
    board->highlightCheck(whiteTurn, opponentInCheck);

    if (opponentInCheck) {
        sounds.playCheck();
    }

    gameState = checkGameState();
    if (gameState == GameState::CHECKMATE || gameState == GameState::STALEMATE) {
        sounds.playEnd();
    }

    promotingPawn = nullptr;
    promotionCaptured = nullptr;
}
