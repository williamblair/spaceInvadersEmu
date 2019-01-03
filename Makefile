# test auto finding cpp files

CPP = g++
CC = gcc

CPPSOURCES = $(wildcard src/*.cpp)
CSOURCES = $(wildcard data/*.c)
OBJECTS = $(CPPSOURCES:.cpp=.o) $(CSOURCES:.c=.o)

BIN = SpaceInvaders

all: $(OBJECTS)
	$(CPP) $(OBJECTS) -o $(BIN)

.cpp.o:
	$(CPP) -c -o $@ $<

.c.o:
	$(CC) -c -o $@ $<

clean:
	rm -f $(OBJECTS)

