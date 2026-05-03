cmake_minimum_required(VERSION 4.0)
project(ProjectOS C)

set(CMAKE_C_STANDARD 11)

add_executable(ProjectOS main.c
        Graph.c
        Graph.h
        DijkstraRes.c
        DijkstraRes.h
        Node.h
        Edge.h
        InputData.h
        MinHeap.h
        MinHeap.c)
