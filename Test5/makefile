.PHONY:all

SRC = $(wildcard *.c)
BIN = $(SRC:%.c=%)
CPPSRC = $(wildcard *.cpp)
CPPBIN = $(CPPSRC:%.cpp=%)

CC = gcc
CXX = g++
CFLAGS = -g -Wall
CXXFLAGS = -g -Wall -std=c++11

all:$(BIN) $(CPPBIN)