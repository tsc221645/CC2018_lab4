CXX = g++
CXXFLAGS = -std=c++17 -Wall -I. `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`
SRC = main.cpp framebuffer.cpp triangle.cpp objloader.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = renderer

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
