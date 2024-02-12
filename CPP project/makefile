#!/bin/make

CC=g++ -std=c++17
CFLAGS=-Wall -Wextra -pedantic -Weffc++ -Wold-style-cast
VFLAGS = -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system

SOURCEDIR = ./src
BUILDDIR = ./build

EXECUTABLE = play
SOURCES = $(wildcard $(SOURCEDIR)/*.cc)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cc,$(BUILDDIR)/%.o,$(SOURCES))


all: $(EXECUTABLE) 
	dir $(BUILDDIR)
	
dir: 
	mkdir -p $(BUILDDIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(VFLAGS)

$(OBJECTS): $(BUILDDIR)/%.o: $(SOURCEDIR)/%.cc

	$(CC) $(CFLAGS) $< -c -o $@ $(VFLAGS)

.PHONY: clean

clean:
	rm -f $(BUILDDIR)/*.o $(BUILDDIR)/$(EXECUTABLE)

run: 
	./play