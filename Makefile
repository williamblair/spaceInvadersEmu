# test auto finding cpp files

CPP = g++
CC = gcc

CFLAGS = -g

LIBS = -lSDL

CPPSOURCES = $(wildcard src/*.cpp)
CSOURCES = $(wildcard data/*.c)
OBJECTS = $(CPPSOURCES:.cpp=.o) $(CSOURCES:.c=.o)

BIN = SpaceInvaders

all: $(OBJECTS)
	$(CPP) $(OBJECTS) -o $(BIN) $(LIBS)

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS)

