#ifndef PROJECTOS_NODE_H
#define PROJECTOS_NODE_H

#include "Edge.h"
#include <pthread.h>

typedef struct Node {
    int id;
    Edge *adj;

    pthread_mutex_t node_mutex;
    int occupying_traveler_id;
} Node;

#endif //PROJECTOS_NODE_H