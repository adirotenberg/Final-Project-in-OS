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
#include <fcntl.h>

void freeAll(InputData* data, bool freeResArr, DijkstraRes ** resArr, int resAmount);

static int getEdgeWeight(Graph *graph, int src, int dst) {
    Edge *curr = graph->vertices[src].adj;

    while (curr != NULL) {
        if (curr->dst == dst) {
            return curr->weight;
        }
        curr = curr->next;
    }

    return 1;
}

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


//    DijkstraRes **resArr = malloc(sizeof(DijkstraRes *) * data->numOfTravelers);
//
//    if (resArr == NULL) {
//        printf("Error: Failed to create res array\n");
//        freeAll(data, false, NULL, 0);
//        exit(EXIT_FAILURE);
//    }
//
//    for (int i = 0; i < data->numOfTravelers; i++) {
//        int src = data->travelers[i][0];
//        int dst = data->travelers[i][1];
//
//        printf("\nDijkstra from %d to %d\n\n", src, dst);
//
//        DijkstraRes *res = dijkstra(data->graph, src, dst);
//
//        if (res == NULL) {
//            printf("Dijkstra failed.\n");
//            // Cleanup previous results
//            freeAll(data, true, resArr, i);
//            return 1;
//        }
//
//        resArr[i] = res;
//        printPath(res);
//    }

    pid_t *pids = malloc(sizeof(pid_t) * data->numOfTravelers);

    int (*pipes)[2] = malloc(sizeof(int[2]) * data->numOfTravelers);

    if (pipes == NULL) {
        printf("Error: Failed to create pipes array\n");
        freeAll(data, false, NULL, 0);
        free(pids);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < data->numOfTravelers; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            freeAll(data, false, NULL, 0);
            free(pids);
            free(pipes);
            exit(EXIT_FAILURE);
        }
    }

    if (pids == NULL) {
        printf("Error: Failed to create pid_t array\n");
        freeAll(data, false, NULL, 0);
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
            freeAll(data, false, NULL, 0);
            free(pids);
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // child process
            close(pipes[i][0]); // child does not read

            int src = data->travelers[i][0];
            int dst = data->travelers[i][1];

            DijkstraRes *res = dijkstra(data->graph, src, dst);

            if (res == NULL) {
                close(pipes[i][1]);
                exit(EXIT_FAILURE);
            }

            for (int j = 0; j < res->pathLength; j++) {
                TravelMessage msg;

                msg.pid = getpid();
                msg.travelerIndex = i;
                msg.currentNode = res->path[j];
                msg.finished = (j == res->pathLength - 1);

                if (msg.finished)
                    msg.nextNode = -1;
                else
                    msg.nextNode = res->path[j + 1];

                write(pipes[i][1], &msg, sizeof(TravelMessage));

//                if (!msg.finished) {
//                    int weight = getEdgeWeight(data->graph, msg.currentNode, msg.nextNode);
////                    usleep(weight * 300000);
////
////                    if (j != 0 && j != res->pathLength - 2) {
////                        sleep(1);
////                    }
//                }
            }

            free(res->path);
            free(res);

            close(pipes[i][1]);
            exit(EXIT_SUCCESS);
        }

        pids[i] = pid;
        close(pipes[i][1]); // parent does not write
        fcntl(pipes[i][0], F_SETFL, O_NONBLOCK);
    }

    //simulation(data, resArr, pids, data->numOfTravelers); // only until milestone 4

    simulation(data, pipes, pids, data->numOfTravelers);

    // Wait for all children
    for (int i = 0; i < data->numOfTravelers; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    // cleanup
    for (int i = 0; i < data->numOfTravelers; i++) {
        close(pipes[i][0]);
    }
    free(pipes);

    freeAll(data, false, NULL, 0);
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