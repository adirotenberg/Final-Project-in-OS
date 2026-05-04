#include "Graph.h"


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

    if (numOfVertices <= 0 || numOfEdges <= 0) {
        printf("Error: Number of vertices and Edges must be positive\n");
        fclose(fp);
        return NULL;
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
            dst < 0 || dst >= graph->numOfVertices ||
            weight < 0) {

            printf("Error: Invalid input\n");
            freeGraph(graph);
            fclose(fp);
            return NULL;
        }

        if (!doesEdgeExists(graph, src, dst)) {
            addEdge(graph, src, dst, weight);
        }
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
        free(graph);
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

    if (graph->vertices == NULL) {
        free(graph);
        return;
    }

    for (int i = 0; i < numOfVertices; i++) {

        Edge *curr = graph->vertices[i].adj;

        Edge *next = NULL;

        while (curr != NULL) {
            next = curr->next;
            free(curr);
            curr = next;
        }
    }

    free(graph->vertices);
    free(graph);
}

bool doesEdgeExists(Graph *graph, int src, int dst) {

    Node curr = graph->vertices[src];

    Edge *currEdge = curr.adj;

    while (currEdge != NULL) {
        if (currEdge->dst == dst) {
            return true;
        }
        currEdge = currEdge->next;
    }
    return false;
}
