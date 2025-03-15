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

## TODO:

- Validations:
  - Check
  - Checkmate
  - Castle (pieces scoping castle squares)
