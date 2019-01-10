# test auto finding cpp files

CPP = c++
CC = cc

CFLAGS = -g

INCDIRS = -I/usr/local/include
LIBDIRS = -L/usr/local/lib

LIBS = -lSDL

CPPSOURCES = $(wildcard src/*.cpp)
CSOURCES = $(wildcard data/*.c)
OBJECTS = $(CPPSOURCES:.cpp=.o) $(CSOURCES:.c=.o)

BIN = SpaceInvaders

all: $(OBJECTS)
	$(CPP) $(OBJECTS) -o $(BIN) $(LIBDIRS) $(LIBS)

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $< $(INCDIRS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $< $(INCDIRS)

clean:
	rm -f $(OBJECTS)

