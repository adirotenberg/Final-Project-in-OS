cmake_minimum_required(VERSION 4.0)
project(ProjectOS C)

set(CMAKE_C_STANDARD 11)

set(RAYLIB_ROOT "./external/raylib")
set(RAYLIB_INCLUDE_DIR "${RAYLIB_ROOT}/include")
set(RAYLIB_LIB_DIR "${RAYLIB_ROOT}/lib")


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
        MinHeap.c
        vizHelperFuncs.h
        vizHelperFuncs.c
        vizGraph.h
        vizGraph.c
)


target_include_directories(ProjectOS PUBLIC "${RAYLIB_INCLUDE_DIR}")
target_link_directories(ProjectOS PUBLIC "${RAYLIB_LIB_DIR}")
target_link_libraries(ProjectOS
        raylib
        opengl32
        gdi32
        winmm
)