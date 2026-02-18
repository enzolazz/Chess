# Chess game in C++

## Dependencies to be installed:

- CMake
- Any C++ compiler (gcc, clang, MSVC, ...)
- Any Build system (make, ninja, ...)
- SFML 3.X graphics interface

## Compiling the project

If you're not using an IDE, compile using the following commands:

```shell
# Generate build files
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release # or Debug

# Compile
cmake --build build
```

To execute the program, run:

```shell
./build/ChessApp
```

## Controls

| Key | Action |
|-----|--------|
| Left Click + Drag | Move piece |
| Right Click | Cancel move |
| ← | Undo |
| → | Redo |
| R | Flip board |
| N | New game |
| Esc | Quit |

## Features

- [x] Basic piece movement
- [x] Check detection and highlight
- [x] Checkmate and stalemate detection
- [x] Castling with proper validation
- [x] En passant
- [x] Pawn promotion with UI
- [x] Move indicators (legal moves shown when dragging)
- [x] Undo/Redo system
- [x] Sound effects

## TODO

### UI Improvements
- [ ] Turn indicator (show whose turn it is)
- [ ] Board coordinates (a-h, 1-8)
- [ ] Captured pieces display
- [ ] Move history with algebraic notation
- [ ] Highlight selected piece's original square

### Code Structure
- [ ] Replace `move_tuple` with proper `Move` class
- [ ] Use `PieceType` enum instead of char
- [ ] Extract `MoveValidator` class
- [ ] Encapsulate Board internal state
- [ ] Centralize color/size constants

### Advanced Rules
- [ ] 50 move rule (draw)
- [ ] Threefold repetition (draw)
- [ ] Insufficient material detection (draw)
- [ ] Chess clock/timer
