#ifndef PROJECTOS_DIJKSTRA_H
#define PROJECTOS_DIJKSTRA_H
#include "Graph.h"


typedef struct DijkstraRes {

    int * path;
    int pathLength;
    int pathWeight;

}DijkstraRes;

DijkstraRes *dijkstra(Graph *graph, int src, int dst);

#endif //PROJECTOS_DIJKSTRA_H
