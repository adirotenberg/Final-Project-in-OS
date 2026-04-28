#ifndef PROJECTOS_GRAPH_H
#define PROJECTOS_GRAPH_H

#include "Node.h"
#include "InputData.h"

typedef struct Graph {

    Node *vertices; //array of nodes
    int numOfVertices;

} Graph;

InputData *readFile(const char *fileName);

Graph *createGraph(int numOfVertices);

void addNode(Graph *graph, int id);

void addEdge(Graph *graph, int src, int dst, int weight);

void freeGraph(Graph *graph);

#endif //PROJECTOS_GRAPH_H
