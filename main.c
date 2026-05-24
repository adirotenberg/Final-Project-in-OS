#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include  <unistd.h>
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


    DijkstraRes **resArr = malloc(sizeof(DijkstraRes *) * data->numOfTravelers); // todo - free ()

    if (resArr == NULL) {
        printf("Error: Failed to create res array\n");
        // todo - cleanup ()
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < data->numOfTravelers; i++) {
        int src = data->travelers[i][0];
        int dst = data->travelers[i][1];

        printf("\nDijkstra from %d to %d\n\n", src, dst);

        // RUN DIJKSTRA
        DijkstraRes *res = dijkstra(data->graph, src, dst);

        if (res == NULL) {
            printf("Dijkstra failed.\n");

            freeGraph(data->graph);
            free(data);
            return 1;
        }

        resArr[i] = res;
        // PRINT RESULT
        printPath(res);
    }

    //Create children

    pid_t pid;
    pid_t *pids = malloc(sizeof(pid_t) * data->numOfTravelers); //todo - free()

    if (pids == NULL) {
        printf("Error: Failed to create pid_t array\n");
        // todo - cleanup ()
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < data->numOfTravelers; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Error: fork failed");
            //todo - cleanup ()
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            printf("%d started", getpid());
            while (1);
            return EXIT_SUCCESS;
        }

        //father
        pids[i] = pid;
    }


    //all the simulation part will be run only if we're not running "milestone1". when we are running milestone1 we'll get the flag DIJKSTRA_ONLY
#ifndef DIJKSTRA_ONLY
    // simulation(data, resArr, pids, data->numOfTravelers);
#endif

    // cleanup
    // free(res->path);
    // free(res);

    freeGraph(data->graph);
    free(data->travelers);
    free(data);

    return 0;
}
