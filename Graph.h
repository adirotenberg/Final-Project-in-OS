#ifndef PROJECTOS_GRAPH_H
#define PROJECTOS_GRAPH_H

typedef struct DijkstraRes DijkstraRes;

#include "Node.h"
#include "InputData.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Graph {

    Node *vertices; //array of nodes
    int numOfVertices;

} Graph;

InputData *readFile(const char *fileName);

Graph *createGraph(int numOfVertices);

void addNode(Graph *graph, int id);

void addEdge(Graph *graph, int src, int dst, int weight);

void freeGraph(Graph *graph);

bool doesEdgeExists(Graph *graph, int src, int dst);

#endif //PROJECTOS_GRAPH_H
