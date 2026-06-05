#ifndef PROJECTOS_SIMULATION_H
#define PROJECTOS_SIMULATION_H

#include "DijkstraRes.h"
#include "InputData.h"
#include "vizGraph.h"
#include <sys/types.h>
static const Color simBgCol = (Color){18, 18, 24, 255};

typedef struct {
    int currentEdgeIndex;
    int currentStep;
    float timer;
    float waitTimer;
    bool isWaitingAtNode;
    bool finished;
    Color color;
    bool signalSent;

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

void simulation(InputData* data, int pipes[][2], pid_t* pids, int numOfTravelers);

#endif //PROJECTOS_SIMULATION_H
