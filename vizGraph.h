//
// Created by yaara on 5/1/2026.
//

#ifndef PROJECTOS_VIZGRAPH_H
#define PROJECTOS_VIZGRAPH_H

#include "vizHelperFuncs.h"
#include "Edge.h"
#include "Graph.h"

void drawGraph(Graph* graph);
void layout(Graph* graph, Vector2 pos[], bool has_edge[][MAX_VERTICES]);
void drawEdges(Graph* graph, Vector2 pos[], bool has_edge[][MAX_VERTICES]);
void drawVertices(int numVertices, Vector2 pos[]);

#endif //PROJECTOS_VIZGRAPH_H