#include "Graph.h"

static void freeAll(Graph *graph, FILE *fp, int (*travelers)[TRAVELER_FIELDS]);

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
            freeAll(graph, fp, NULL);
            return NULL;
        }

        if (src < 0 || src >= graph->numOfVertices ||
            dst < 0 || dst >= graph->numOfVertices ||
            weight < 0) {
            printf("Error: Invalid input\n");
            freeAll(graph, fp, NULL);
            return NULL;
        }

        if (!doesEdgeExists(graph, src, dst)) {
            addEdge(graph, src, dst, weight);
        }
    }

    int numOfTravelers;

    int travelersNumScan = fscanf(fp, "%d", &numOfTravelers);

    if (travelersNumScan != 1) {
        printf("Error: Invalid travelers number row format\n");
        freeAll(graph, fp, NULL);
        return NULL;
    }

    int (*travelers)[TRAVELER_FIELDS] = malloc(sizeof(int[TRAVELER_FIELDS]) * numOfTravelers);

    if (travelers == NULL) {
        printf("Error: Failed to create travelers array\n");
        freeAll(graph, fp, NULL);
        return NULL;
    }

    // Skip to the start of traveler lines by reading character-by-character until after the newline of travelersNumScan
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n');

    for (int i = 0; i < numOfTravelers; i++) {
        char line[256];
        // Read next non-empty, non-whitespace-only line
        while (1) {
            if (fgets(line, sizeof(line), fp) == NULL) {
                printf("Error: Unexpected EOF while reading travelers\n");
                freeAll(graph, fp, travelers);
                return NULL;
            }
            // Check if line is empty or just whitespace
            int only_whitespace = 1;
            for (int k = 0; line[k] != '\0'; k++) {
                if (line[k] != ' ' && line[k] != '\t' && line[k] != '\r' && line[k] != '\n') {
                    only_whitespace = 0;
                    break;
                }
            }
            if (!only_whitespace) {
                break;
            }
        }

        int dijkSrc;
        int dijkDst;
        int priority = -1;
        int scanRow = sscanf(line, "%d %d %d", &dijkSrc, &dijkDst, &priority);

        if (scanRow < 2) {
            printf("Error: Invalid row format for traveler number #%d\n", i);
            freeAll(graph, fp, travelers);
            return NULL;
        }

        // If priority is not provided in the file, assign a default unique priority.
        if (scanRow == 2 || priority == -1) {
            priority = i + 1;
        }

        if (dijkSrc < 0 || dijkSrc >= graph->numOfVertices ||
            dijkDst < 0 || dijkDst >= graph->numOfVertices ||
            priority < 1 || priority > numOfTravelers + 1
            ) {
            printf("Error: Invalid source/destination/priority (%d %d %d) for traveler number #%d\n", dijkSrc, dijkDst, priority, i);
            freeAll(graph, fp, travelers);
            return NULL;
        }

        travelers[i][0] = dijkSrc;
        travelers[i][1] = dijkDst;
        travelers[i][2] = priority;
    }

    InputData *inputData = malloc(sizeof(InputData));

    if (inputData == NULL) {
        printf("Error: Failed to create input data\n");
        freeAll(graph, fp, travelers);
        return NULL;
    }

    inputData->graph = graph;
    inputData->travelers = travelers;
    inputData->numOfTravelers = numOfTravelers;

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

static void freeAll(Graph *graph, FILE *fp, int (*travelers)[TRAVELER_FIELDS]) {
    freeGraph(graph);
    fclose(fp);
    if (travelers != NULL) {
        free(travelers);
    }
}