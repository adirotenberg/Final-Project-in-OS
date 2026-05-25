#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
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


    DijkstraRes **resArr = malloc(sizeof(DijkstraRes *) * data->numOfTravelers);

    if (resArr == NULL) {
        printf("Error: Failed to create res array\n");
        freeGraph(data->graph);
        free(data->travelers);
        free(data);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < data->numOfTravelers; i++) {
        int src = data->travelers[i][0];
        int dst = data->travelers[i][1];

        printf("\nDijkstra from %d to %d\n\n", src, dst);

        DijkstraRes *res = dijkstra(data->graph, src, dst);

        if (res == NULL) {
            printf("Dijkstra failed.\n");
            // Cleanup previous results
            for (int j = 0; j < i; j++) {
                free(resArr[j]->path);
                free(resArr[j]);
            }
            free(resArr);
            freeGraph(data->graph);
            free(data->travelers);
            free(data);
            return 1;
        }

        resArr[i] = res;
        printPath(res);
    }

    pid_t *pids = malloc(sizeof(pid_t) * data->numOfTravelers);

    if (pids == NULL) {
        printf("Error: Failed to create pid_t array\n");
        for (int i = 0; i < data->numOfTravelers; i++) {
            free(resArr[i]->path);
            free(resArr[i]);
        }
        free(resArr);
        freeGraph(data->graph);
        free(data->travelers);
        free(data);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < data->numOfTravelers; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Error: fork failed");
            // Kill already created children
            for (int j = 0; j < i; j++) {
                kill(pids[j], SIGKILL);
            }
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            printf("[%d] started\n", getpid());
            while (1) {
                pause();
            }
            return EXIT_SUCCESS;
        }

        pids[i] = pid;
    }


#ifndef DIJKSTRA_ONLY
    simulation(data, resArr, pids, data->numOfTravelers);
#endif

    // Wait for all children
    for (int i = 0; i < data->numOfTravelers; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    // cleanup
    for (int i = 0; i < data->numOfTravelers; i++) {
        free(resArr[i]->path);
        free(resArr[i]);
    }
    free(resArr);
    free(pids);

    freeGraph(data->graph);
    free(data->travelers);
    free(data);

    return 0;
}
