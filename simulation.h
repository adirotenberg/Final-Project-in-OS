#ifndef PROJECTOS_SIMULATION_H
#define PROJECTOS_SIMULATION_H

#include "DijkstraRes.h"
#include "InputData.h"
#include "vizGraph.h"
#include <sys/types.h>

void simulation(InputData *data, DijkstraRes **dijkstra_res_arr, pid_t *pids, int numOfTravelers);

#endif //PROJECTOS_SIMULATION_H
