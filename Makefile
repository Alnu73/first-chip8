CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lglut -lGL -lGLU -lGLEW
SOURCES=src/main.cpp src/chip8.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=chip8

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS)