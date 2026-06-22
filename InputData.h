#ifndef PROJECTOS_INPUTDATA_H
#define PROJECTOS_INPUTDATA_H
#define TRAVELER_FIELDS 3

typedef struct {
    struct Graph *graph;
    int (*travelers)[TRAVELER_FIELDS];
    int numOfTravelers;
} InputData;


#endif //PROJECTOS_INPUTDATA_H
