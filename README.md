# Chess game in C++

## Dependências que devem ser instaladas

- CMake
- Algum compilador de C++ (gcc, clang, MSVC, ...)
- Algum Build system (make, ninja, ...)
- Biblioteca SFML 3.X

# Compilar o projeto

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
