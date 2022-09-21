CXX = g++

CXXFLAGS = -std=c++2a -ggdb -Wall -Wextra -Wpedantic

LINK = -lstdc++ -lsfml-graphics -lsfml-window -lsfml-system

SOURCES = $(wildcard src/*.cpp)
DEPS = $(wildcard src/*.h*)
OBJ = $(SOURCES:.cpp=.o)

fieldday: $(OBJ)
	$(CXX) $(FLAGS) -o $@ $^ $(LINK)

.PHONY: clean

clean:
	rm $(OBJ)
