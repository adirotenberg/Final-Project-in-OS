#ifndef PROJECTOS_DIJKSTRA_H
#define PROJECTOS_DIJKSTRA_H
#include "Graph.h"
# define INF 1e9

typedef struct DijkstraRes {

    int * path;
    int pathLength;
    int pathWeight;

}DijkstraRes;

DijkstraRes *dijkstra(Graph *graph, int src, int dst);
void printPath(DijkstraRes *res);

#endif //PROJECTOS_DIJKSTRA_H
