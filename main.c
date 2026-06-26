#define _POSIX_C_SOURCE 200809L //in order for kill to work in c11
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "DijkstraRes.h"
#include "Graph.h"
#include "simulation.h"

void freeAll(InputData* data, pid_t *pids, int (*childToParent)[2],int (*parentToChild)[2]);


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

    pid_t *pids = malloc(sizeof(pid_t) * data->numOfTravelers);

    if (pids == NULL) {
        printf("Error: Failed to create pid_t array\n");
        //freeAll(data, NULL, NULL);
        freeAll(data, NULL, NULL, NULL);
        exit(EXIT_FAILURE);
    }

    //int (*pipes)[2] = malloc(sizeof(int[2]) * data->numOfTravelers);

    int (*childToParent)[2] = malloc(sizeof(int[2]) * data->numOfTravelers);
    int (*parentToChild)[2] = malloc(sizeof(int[2]) * data->numOfTravelers);

    if (childToParent == NULL || parentToChild == NULL) {
        printf("Error: Failed to create pipes arrays\n");
        //freeAll(data, pids, NULL);
        freeAll(data, pids, NULL, NULL);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < data->numOfTravelers; i++) {
        if (pipe(childToParent[i] )==-1|| pipe(parentToChild[i]) == -1) {
            perror("pipe failed");
            // Close already opened pipes
            for (int j = 0; j < i; j++) {
                //close(pipes[j][0]);
                //close(pipes[j][1]);
                close(childToParent[j][0]);
                close(childToParent[j][1]);

                close(parentToChild[j][0]);
                close(parentToChild[j][1]);
            }
            //freeAll(data, pids, pipes);
            freeAll(data, pids, childToParent, parentToChild);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < data->numOfTravelers; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Error: fork failed");
            // Kill already created children
            for (int j = 0; j < i; j++) {
                kill(pids[j], SIGKILL);
            }
            // Close all pipes
            // for (int j = 0; j < data->numOfTravelers; j++) {
            //     close(pipes[j][0]);
            //     if (j >= i) close(pipes[j][1]); // parent still has write ends for j >= i
            // }
            
            // for (int j = 0; j < data->numOfTravelers; j++) {
            //     close(pipes[j][0]);
            //     if (j >= i)
            //         close(pipes[j][1]);
            // }

           for (int j = 0; j < data->numOfTravelers; j++) {
                close(childToParent[j][0]);
                close(childToParent[j][1]);

                close(parentToChild[j][0]);
                close(parentToChild[j][1]);
            }
            //freeAll(data, pids, pipes);
            freeAll(data, pids, childToParent,parentToChild);
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // child process
            close(childToParent[i][0]);
            close(parentToChild[i][1]);
            // close other children's pipes ends that were inherited
            // for (int j = 0; j < data->numOfTravelers; j++) {
            //     if (i != j) {
            //         close(pipes[j][0]);
            //         // if j < i, parent already closed pipes[j][1], so it's not inherited?
            //         // actually parent closes it AFTER fork. So child i inherits all pipes[0..i-1][0] (read ends) 
            //         // and all pipes[0..numOfTravelers-1][1] (write ends) that weren't closed yet.
            //         // To be safe, child should close everything it doesn't need.
            //     }
            // }
            // // For simplicity, let's just close what we know.
            // // But we need to close write ends of other pipes too to avoid leaks.
            // for (int j = 0; j < i; j++) {
            //     close(pipes[j][1]);
            // }
            // for (int j = i + 1; j < data->numOfTravelers; j++) {
            //     close(pipes[j][0]);
            //     close(pipes[j][1]);
            // }

        for (int j = 0; j < data->numOfTravelers; j++) {

            if (j == i) {
                // Keep only what this child needs
                close(childToParent[j][0]);   // keep write end
                close(parentToChild[j][1]);   // keep read end
            } else {
                // Close every descriptor belonging to other travelers
                close(childToParent[j][0]);
                close(childToParent[j][1]);

                close(parentToChild[j][0]);
                close(parentToChild[j][1]);
            }
        }

            int src = data->travelers[i][0];
            int dst = data->travelers[i][1];

            DijkstraRes *res = dijkstra(data->graph, src, dst);

            if (res == NULL) {
                //close(pipes[i][1]);
                close(childToParent[i][1]);
                close(parentToChild[i][0]);
                //freeAll(data, pids, pipes);
                freeAll(data, pids, childToParent,parentToChild);
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

                //write(pipes[i][1], &msg, sizeof(TravelMessage));
                write(childToParent[i][1], &msg, sizeof(TravelMessage));

                char ack;
            read(parentToChild[i][0], &ack, sizeof(char));
            }
            freeDijkstraRes(res);

            //close(pipes[i][1]);
            close(childToParent[i][1]);
            close(parentToChild[i][0]);
            //freeAll(data, pids, pipes);
            freeAll(data, pids, childToParent,parentToChild);
            exit(EXIT_SUCCESS);
        }

        pids[i] = pid;
        //close(pipes[i][1]); // parent does not write
        close(childToParent[i][1]);   // parent doesn't write here
        close(parentToChild[i][0]);   // parent doesn't read here
        //fcntl(pipes[i][0], F_SETFL, O_NONBLOCK);
        fcntl(childToParent[i][0], F_SETFL, O_NONBLOCK);
    }

    //simulation(data, pipes, data->numOfTravelers);
    simulation(
    data,
    childToParent,
    parentToChild,
    data->numOfTravelers
 );

    // Wait for all children
    for (int i = 0; i < data->numOfTravelers; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    // cleanup
    for (int i = 0; i < data->numOfTravelers; i++) {
        //close(pipes[i][0]);
        close(childToParent[i][0]);
        close(parentToChild[i][1]);
    }
    
    //freeAll(data, pids, pipes);
    freeAll(data, pids, childToParent,parentToChild);

    return 0;
}

void freeAll(InputData* data, pid_t *pids, int (*childToParent)[2],int (*parentToChild)[2])
{
    if (data != NULL) {
        if (data->graph != NULL) {
            freeGraph(data->graph);
        }
        if (data->travelers != NULL) {
            free(data->travelers);
        }
        free(data);
    }
    if (pids != NULL) {
        free(pids);
    }
    if (childToParent != NULL) {
        free(childToParent);
    }
    if(parentToChild!=NULL){
        free(parentToChild);
    }
}