#ifndef PROJECTOS_SIMULATION_H
#define PROJECTOS_SIMULATION_H

#include "InputData.h"
#include "vizGraph.h"

#define FCFS 0
#define PRIORITY 1

static const Color simBgCol = (Color){18, 18, 24, 255};

typedef struct {
    int currentEdgeIndex;
    int currentStep;
    float timer;
    float waitTimer;
    float edgeProgress; // Added for smooth animation
    bool isWaitingAtNode;
    bool isQueueingOutside;
    bool finished;
    Color color;
    bool signalSent;
    pid_t pid;

    int currentNode;
    int nextNode;
} TravelerState;

typedef struct {
    pid_t pid;
    int travelerIndex;
    int currentNode;
    int nextNode;
    int finished;
} TravelMessage;

void simulation(InputData* data, int pipes[][2], int numOfTravelers, int schd, int ** waitingQueues, int * queuesLengths, pid_t *pids);

#endif //PROJECTOS_SIMULATION_H
