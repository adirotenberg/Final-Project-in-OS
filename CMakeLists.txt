cmake_minimum_required(VERSION 4.0)
project(ProjectOS C)

set(CMAKE_C_STANDARD 11)

add_executable(ProjectOS main.c
        Graph.c
        Graph.h
        DijkstraRes.c
        DijkstraRes.h
        Node.h
        Node.c
        Edge.h
        Edge.c
        InputData.h
        InputData.c
        MinHeap.h
        MinHeap.c)
