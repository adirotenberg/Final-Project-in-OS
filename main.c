#include <stdio.h>
#include <stdlib.h>
#include "Graph.h"
#include "DijkstraRes.h"   // adjust if your file name differs

void printGraph(Graph *graph) {

    if (graph == NULL) return;

    for (int i = 0; i < graph->numOfVertices; i++) {
        printf("Node %d: ", graph->vertices[i].id);

        Edge *curr = graph->vertices[i].adj;
        while (curr != NULL) {
            printf("-> (%d, w=%d) ", curr->dst, curr->weight);
            curr = curr->next;
        }

        printf("\n");
    }
}

void printPath(DijkstraRes *res) {

    if (res == NULL) {
        printf("No result (NULL)\n");
        return;
    }

    printf("Path: ");

    for (int i = 0; i < res->pathLength; i++) {
        printf("%d", res->path[i]);

        if (i < res->pathLength - 1)
            printf(" -> ");
    }

    printf("\nTotal cost: %d\n", res->pathWeight);
}

int main() {

    const char *filename = "input.txt";

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

    // cleanup
    free(res->path);
    free(res);

    freeGraph(data->graph);
    free(data);

    return 0;
}