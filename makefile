# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iheaders

# SFML Libraries
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Source files
SRC = src/pieces/Bishop.cpp src/pieces/King.cpp src/pieces/Knight.cpp src/pieces/Pawn.cpp src/pieces/Piece.cpp src/pieces/Queen.cpp src/pieces/Rook.cpp src/Board.cpp src/Game.cpp src/Main.cpp
# SRC = src/main.cpp
HEADERS = headers/*.h headers/pieces/*.h
OBJ = $(SRC:.cpp=.o)

# Executable name
TARGET = app

# Build target
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	rm -f $(OBJ)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean the project
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean
