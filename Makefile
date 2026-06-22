# Makefile for Operating Systems Project - Traffic Simulation

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
RAYLIB_ROOT = ./external/raylib
RAYLIB_INCLUDE = $(RAYLIB_ROOT)/include
RAYLIB_LIB = $(RAYLIB_ROOT)/lib

SRCS = main.c Graph.c DijkstraRes.c MinHeap.c vizHelperFuncs.c vizGraph.c simulation.c
OBJS = $(SRCS:.c=.o)
TARGET = sim-schd

RAYLIB_LIBS = -L$(RAYLIB_LIB) -lraylib -lm -lpthread -ldl -lrt -lX11
LIBS = -lm

INPUT_FILE ?= input.txt
POLICY ?= fcfs

all: milestone7

milestone7: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -I$(RAYLIB_INCLUDE) $(OBJS) $(RAYLIB_LIBS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -I$(RAYLIB_INCLUDE) -c $< -o $@

clean:
	rm -f *.o $(TARGET) sim

run: $(TARGET)
	./$(TARGET) $(POLICY) $(INPUT_FILE)

.PHONY: all milestone7 clean run