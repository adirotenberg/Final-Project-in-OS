#ifndef PROJECTOS_VIZHELPERFUNCS_H
#define PROJECTOS_VIZHELPERFUNCS_H

#include "external/raylib/include/raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "Graph.h"

/* ── constants ─────────────────────────────────────────────────────────────*/
#define MAX_VERTICES   15
#define MAX_EDGES      200
#define VERTEX_RADIUS  26
#define ARROW_HEAD     13.0f   /* arrowhead "length" in pixels               */
#define ARROW_ANGLE    0.42f   /* half-angle of the arrowhead (radians ≈ 24°) */
#define CURVE_OFFSET   32.0f   /* perpendicular bend for bidirectional edges  */
#define SELF_LOOP_R    22.0f   /* radius of the self-loop circle              */
#define SCREEN_W       800
#define SCREEN_H       600

void DrawEdge(Vector2 from, Vector2 to, int weight, bool curved, Color lineCol, Color textCol);
void DrawSelfLoop(Vector2 pos, int weight, Color lineCol, Color textCol);
int getEdgeWeight(Graph *graph, int src, int dst);
Vector2 getEntityPosition(Vector2 from, Vector2 to, int step, int totalSteps);
Color getRandomColor(int index);

#endif //PROJECTOS_VIZHELPERFUNCS_H