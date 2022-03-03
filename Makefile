CC = gcc
CXX = g++
CXXFLAGS = -D FREEGLUT_STATIC
LDFLAGS = -Wall -Werror -Wextra \
	-O2 -static-libgcc -static -std=c17 \
	-lfreeglut_static -lopengl32 -lwinmm -lgdi32 -lglu32

EXEC = EqnoCraft.exe
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=build/%.o)

all: build $(EXEC)
	@echo COMPILING SUCCEED

build:
	@mkdir build

$(EXEC): $(OBJ)
	@$(CXX) $(OBJ) $(LDFLAGS) -o $@

build/%.o: src/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@