#include "Graph.h"

#include <stdio.h>
#include <stdlib.h>

InputData *readFile(const char *filename) {

    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Error: Failed to open file\n");
        return NULL;
    }

    int numOfVertices;
    int numOfEdges;

    int firstRowScan = fscanf(fp, "%d %d", &numOfVertices, &numOfEdges);

    if (firstRowScan != 2) {
        printf("Error: Invalid first row format\n");
        fclose(fp);
        return NULL;
    }

    if (numOfVertices < 0 || numOfEdges < 0) {
        printf("Error: Number of vertices and Edges must be positive\n");
        fclose(fp);
        return NULL;
        //todo - check case were V=E=0 ?
    }

    Graph *graph = createGraph(numOfVertices);

    if (graph == NULL) {
        printf("Error: Failed to create graph\n");
        fclose(fp);
        return NULL;
    }

    for (int i = 0; i < numOfEdges; i++) {
        int src;
        int dst;
        int weight;

        int scanRow = fscanf(fp, "%d %d %d", &src, &dst, &weight);

        if (scanRow != 3) {
            printf("Error: Invalid row format\n");
            freeGraph(graph);
            fclose(fp);
            return NULL;
        }

        if (src < 0 || src >= graph->numOfVertices ||
            dst < 0 || dst >= graph->numOfVertices) {
            printf("Error: Invalid vertex index\n");
            freeGraph(graph);
            fclose(fp);
            return NULL;
        }

        // if (src == dst) {
        //
        // }
        // todo - check case

        // if (edgeAlreadyExists()) {
        // }
        // todo - check case

        addEdge(graph, src, dst, weight);
    }

    int dijkSrc;
    int dijkDst;

    int lastRowScan = fscanf(fp, "%d %d", &dijkSrc, &dijkDst);

    if (lastRowScan != 2) {
        printf("Error: Invalid last row format\n");
        freeGraph(graph);
        fclose(fp);
        return NULL;
    }

    if (dijkSrc < 0 || dijkSrc >= graph->numOfVertices ||
    dijkDst < 0 || dijkDst >= graph->numOfVertices) {

        freeGraph(graph);
        fclose(fp);
        return NULL;
        //todo- check case
    }

    InputData *inputData = malloc(sizeof(InputData));

    if (inputData == NULL) {
        printf("Error: Failed to create input data\n");
        freeGraph(graph);
        fclose(fp);
        return NULL;
    }

    inputData->graph = graph;
    inputData->src = dijkSrc;
    inputData->dst = dijkDst;

    fclose(fp);
    return inputData;
}

Graph *createGraph(int numOfVertices) {

    Graph *graph = malloc(sizeof(Graph));

    if (graph == NULL) {
        return NULL;
    }

    graph->numOfVertices = numOfVertices;

    graph->vertices = malloc(sizeof(Node) * numOfVertices);

    if (graph->vertices == NULL) {
        free(graph); //todo - might be able to use freeGraph(graph)
        return NULL;
    }

    for (int i = 0; i < numOfVertices; i++) {
        graph->vertices[i].id = i;
        graph->vertices[i].adj = NULL;
    }

    return graph;

}

void addEdge(Graph *graph, int src, int dst, int weight) {

    Edge *newEdge = (Edge *) malloc(sizeof(Edge));

    if (newEdge == NULL) {
        return;
    }

    newEdge->dst = dst;
    newEdge->weight = weight;
    newEdge->next = NULL;

    Edge *curr = graph->vertices[src].adj;
    Edge *prev = NULL;

    if (curr == NULL) {
        graph->vertices[src].adj = newEdge;
        return;
    }

    while (curr != NULL) {
        if (curr->dst == dst) {
            free(newEdge);
            return;
        }
        prev = curr;
        curr = curr->next;

    }

    prev->next = newEdge;

}

void freeGraph(Graph *graph) {

    if (graph == NULL) {
        return;
    }

    int numOfVertices = graph->numOfVertices;

    for (int i = 0; i < numOfVertices; i++) {

        Edge *curr = graph->vertices[i].adj;
        Edge *next = NULL;

        while (curr != NULL) {
            next = curr->next;
            free(curr);
            curr = next;
        }
    }

    if (graph->vertices != NULL) {
        free(graph->vertices);
    }

    free(graph);

}
