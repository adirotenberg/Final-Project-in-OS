#ifndef PROJECTOS_NODE_H
#define PROJECTOS_NODE_H

#include "Edge.h"

typedef struct Node {

    int id;
    Edge *adj; //head of linked list containing node's edges

} Node;


#endif //PROJECTOS_NODE_H
