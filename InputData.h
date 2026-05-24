#ifndef PROJECTOS_INPUTDATA_H
#define PROJECTOS_INPUTDATA_H

typedef struct {
    struct Graph *graph;
    int (*travelers)[2];
    int numOfTravelers;
} InputData;


#endif //PROJECTOS_INPUTDATA_H
