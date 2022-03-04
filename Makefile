CXX = g++
LD = g++
CXXFLAGS = -Iinclude/ -DFREEGLUT_STATIC -Wall -Wextra \
	-O2 -std=c++17
LDFLAGS = -static -static-libgcc \
	-lfreeglut_static -lopengl32 -lwinmm -lgdi32 -lglu32

EXEC = EqnoCraft.exe
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=build/%.o)

.PHONY: all clean

all: build $(EXEC)

build:
	@mkdir build

$(EXEC): $(OBJ)
	@echo LD    $@
	@$(LD) $(OBJ) $(LDFLAGS) -o $@

build/%.o: src/%.cpp
	@echo CXX   $<
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo CLEAN build $(EXEC)
	@-del /F /Q $(EXEC) 2> nul
	@-rmdir /S /Q build 2> nul
	@echo FINISH
