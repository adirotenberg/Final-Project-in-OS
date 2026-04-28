#include "DijkstraRes.h"

#include <stdio.h>

#include "MinHeap.h"
#include <stdlib.h>

DijkstraRes *dijkstra(Graph *graph, int src, int dst) {

    int n = graph->numOfVertices;

    int *dist = malloc(sizeof(int) * n);

    if (dist == NULL) {
        printf("Error: Failed to create distance array");
        return NULL;
    }

    int *parent = malloc(sizeof(int) * n);

    if (parent == NULL) {
        printf("Error: Failed to create parent array");
        free(dist);
        return NULL;
    }

    int *visited = calloc(n, sizeof(int)); // allocate memory and sets everything to 0

    if (visited == NULL) {
        printf("Error: Failed to create visited array");
        free(dist);
        free(parent);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        dist[i] = 1e9;
        parent[i] = -1;
    }

    MinHeap *heap = createHeap(n * n);

    dist[src] = 0;
    insert(heap, src, 0);

    while (heap->size > 0) {

        HeapNode curr = extractMin(heap);
        int u = curr.node;

        if (visited[u]) continue;
        visited[u] = 1;

        if (u == dst) break;

        Edge *edge = graph->vertices[u].adj;

        while (edge != NULL) {

            int v = edge->dst;
            int w = edge->weight;

            if (!visited[v] && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                insert(heap, v, dist[v]);
            }

            edge = edge->next;
        }
    }

    int *temp = malloc(sizeof(int) * n);

    if (temp == NULL) {
        printf("Error: Failed to create temporary array");
        free(dist);
        free(parent);
        free(visited);
        return NULL;
    }

    int len = 0;

    int curr = dst;

    while (curr != -1) {
        temp[len++] = curr;
        curr = parent[curr];
    }

    int *path = malloc(sizeof(int) * len);

    if (path == NULL) {
        printf("Error: Failed to create path array");
        free(dist);
        free(parent);
        free(visited);
        free(temp);
        return NULL;
    }

    for (int i = 0; i < len; i++) {
        path[i] = temp[len - i - 1];
    }

    DijkstraRes *res = malloc(sizeof(DijkstraRes));

    if (res == NULL) {
        printf("Error: Failed to create result");
        free(dist);
        free(parent);
        free(visited);
        free(temp);
        free(path);
        return NULL;
    }
    res->path = path;
    res->pathLength = len;
    res->pathWeight = dist[dst];

    free(temp);
    free(dist);
    free(parent);
    free(visited);
    freeHeap(heap);

    return res;
}
