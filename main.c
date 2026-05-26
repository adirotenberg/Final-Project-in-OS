#define _POSIX_C_SOURCE 200809L //in order for kill to work in c11
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "DijkstraRes.h"
#include "Graph.h"
#include "simulation.h"

void freeAll(InputData* data, bool freeResArr, DijkstraRes ** resArr, int resAmount);

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
        freeAll(data, false, NULL, 0);
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
            freeAll(data, true, resArr, i);
            return 1;
        }

        resArr[i] = res;
        printPath(res);
    }

    pid_t *pids = malloc(sizeof(pid_t) * data->numOfTravelers);

    if (pids == NULL) {
        printf("Error: Failed to create pid_t array\n");
        freeAll(data, true, resArr, data->numOfTravelers);
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
            freeAll(data, true, resArr, data->numOfTravelers);
            free(pids);
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

    simulation(data, resArr, pids, data->numOfTravelers);

    // Wait for all children
    for (int i = 0; i < data->numOfTravelers; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    // cleanup
    freeAll(data, true, resArr, data->numOfTravelers);
    free(pids);

    return 0;
}

void freeAll(InputData* data, bool freeResArr, DijkstraRes ** resArr, int resAmount)
{
    if (data == NULL) return;

    if (freeResArr && resArr != NULL)
    {
        for (int j = 0; j < resAmount; j++) {
            if (resArr[j] != NULL) {
                free(resArr[j]->path);
                free(resArr[j]);
            }
        }
        free(resArr);
    }
    if (data->graph != NULL) {
        freeGraph(data->graph);
    }
    if (data->travelers != NULL) {
        free(data->travelers);
    }
    free(data);
}