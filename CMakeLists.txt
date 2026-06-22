cmake_minimum_required(VERSION 4.0)
project(ProjectOS C)

set(CMAKE_C_STANDARD 11)

set(RAYLIB_ROOT "./external/raylib")
set(RAYLIB_INCLUDE_DIR "${RAYLIB_ROOT}/include")
set(RAYLIB_LIB_DIR "${RAYLIB_ROOT}/lib")


add_executable(sim-schd main.c
        Graph.c
        Graph.h
        DijkstraRes.c
        DijkstraRes.h
        Node.h
        Edge.h
        InputData.h
        MinHeap.h
        MinHeap.c
        vizHelperFuncs.h
        vizHelperFuncs.c
        vizGraph.h
        vizGraph.c
        simulation.c
        simulation.h
)


target_include_directories(sim-schd PUBLIC "${RAYLIB_INCLUDE_DIR}")
target_link_directories(sim-schd PUBLIC "${RAYLIB_LIB_DIR}")
target_link_libraries(sim-schd
        raylib
        opengl32
        gdi32
        winmm
)