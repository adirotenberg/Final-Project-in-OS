# Makefile for Operating Systems Project - Traffic Simulation

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11 $(shell pkg-config --cflags raylib)
RAYLIB_ROOT = ./external/raylib
RAYLIB_INCLUDE = $(RAYLIB_ROOT)/include
RAYLIB_LIB = $(RAYLIB_ROOT)/lib

SRCS = main.c Graph.c DijkstraRes.c MinHeap.c vizHelperFuncs.c vizGraph.c simulation.c
OBJS = $(SRCS:.c=.o)
TARGET = sim

RAYLIB_LIBS = $(shell pkg-config --libs raylib) -lm -lX11
LIBS = -lm

INPUT_FILE ?= input.txt

all: milestone3

milestone1: dijkstra

milestone2: $(TARGET)

milestone3: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -I$(RAYLIB_INCLUDE) $(OBJS) $(RAYLIB_LIBS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -I$(RAYLIB_INCLUDE) -c $< -o $@

dijkstra: dijkstra.o Graph.o DijkstraRes.o MinHeap.o
	$(CC) $(CFLAGS) -I$(RAYLIB_INCLUDE) dijkstra.o Graph.o DijkstraRes.o MinHeap.o -o dijkstra $(LIBS)

dijkstra.o: main.c Graph.h DijkstraRes.h Node.h Edge.h InputData.h MinHeap.h
	$(CC) $(CFLAGS) -I$(RAYLIB_INCLUDE) -DDIJKSTRA_ONLY -c main.c -o dijkstra.o

clean:
	rm -f *.o $(TARGET) dijkstra

run: $(TARGET)
	./$(TARGET) $(INPUT_FILE)

run-m1: dijkstra
	./dijkstra $(INPUT_FILE)

.PHONY: all milestone1 milestone2 milestone3 clean run run-m1