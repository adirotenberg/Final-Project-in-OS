#include <stdio.h>
#include <stdlib.h>
#include "DijkstraRes.h"
#include "Graph.h"
#include "simulation.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    InputData *data = readFile(filename);

    if (data == NULL) {
        printf("Failed to read file.\n");
        return 1;
    }

    printf("Graph loaded successfully!\n\n");

    printGraph(data->graph);

    printf("\nDijkstra from %d to %d\n\n", data->src, data->dst);

    // RUN DIJKSTRA
    DijkstraRes *res = dijkstra(data->graph, data->src, data->dst);

    if (res == NULL) {
        printf("Dijkstra failed.\n");

        freeGraph(data->graph);
        free(data);
        return 1;
    }

    // PRINT RESULT
    printPath(res);

    //all the simulation part will be run only if we're not running "milestone1". when we are running milestone1 we'll get the flag DIJKSTRA_ONLY
#ifndef DIJKSTRA_ONLY
    simulation(data, res);
#endif

    // cleanup
    free(res->path);
    free(res);

    freeGraph(data->graph);
    free(data);

    return 0;
}