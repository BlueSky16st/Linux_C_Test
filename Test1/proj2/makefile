.PHONY:clean all

SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=%.o)
BIN = $(SRC:%.cpp=%)
CPP = g++
CPPFlags = -g -Wall

all:$(BIN)

$(BIN):%:%.o
	$(CPP) $(CPPFlags) $^ -o $@

%.o:%.cpp
	$(CPP) $(CPPFlags) -c $^ -o $@

clean:
	rm -rf $(OBJ) $(BIN)

