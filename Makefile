# Makefile for Operating Systems Project - Traffic Simulation

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
RAYLIB_ROOT = ./external/raylib
RAYLIB_INCLUDE = $(RAYLIB_ROOT)/include
RAYLIB_LIB = $(RAYLIB_ROOT)/lib

SRCS = main.c Graph.c DijkstraRes.c MinHeap.c vizHelperFuncs.c vizGraph.c simulation.c
OBJS = $(SRCS:.c=.o)
TARGET = sim

RAYLIB_LIBS = -L$(RAYLIB_LIB) -lraylib -lm -lpthread -ldl -lrt -lX11
LIBS = -lm

INPUT_FILE ?= input.txt

all: milestone4

milestone4: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -I$(RAYLIB_INCLUDE) $(OBJS) $(RAYLIB_LIBS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -I$(RAYLIB_INCLUDE) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(INPUT_FILE)

.PHONY: all milestone4 clean run