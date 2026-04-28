#ifndef PROJECTOS_EDGE_H
#define PROJECTOS_EDGE_H

typedef struct Edge {

    int dst;
    int weight;
    struct Edge *next;

} Edge;

#endif //PROJECTOS_EDGE_H
