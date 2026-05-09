#include "simulation.h"
#include "vizGraph.h"
#include <stdbool.h>

static int getEdgeWeight(Graph *graph, int src, int dst) {
    Edge *curr = graph->vertices[src].adj;

    while (curr != NULL) {
        if (curr->dst == dst) {
            return curr->weight;
        }
        curr = curr->next;
    }

    return 1;
}

static Vector2 getEntityPosition(Vector2 from, Vector2 to, int step, int totalSteps) {
    float t = (float)step / (float)totalSteps;

    Vector2 pos;
    pos.x = from.x + (to.x - from.x) * t;
    pos.y = from.y + (to.y - from.y) * t;

    return pos;
}

void simulation(InputData* data, DijkstraRes* dijkstra_res) {
    Graph *graph = data->graph;

    Vector2 pos[MAX_VERTICES];
    bool has_edge[MAX_VERTICES][MAX_VERTICES] = {0};

    layout(graph, pos, has_edge);

    int currentEdgeIndex = 0;
    int currentStep = 0;
    float timer = 0.0f;
    float waitTimer = 0.0f;

    bool isPlaying = false;
    bool isWaitingAtNode = false;
    bool finished = false;

    Rectangle playButton = {20, 45, 120, 40};

    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(SCREEN_W, SCREEN_H, "Traffic Simulation - Milestone 3");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();

            if (CheckCollisionPointRec(mouse, playButton)) {
                isPlaying = !isPlaying;
            }
        }

        if (isPlaying && !finished && dijkstra_res->pathLength > 1) {
            if (isWaitingAtNode) {
                waitTimer += dt;

                if (waitTimer >= 1.0f) {
                    waitTimer = 0.0f;
                    isWaitingAtNode = false;
                }
            } else {
                timer += dt;

                int src = dijkstra_res->path[currentEdgeIndex];
                int dst = dijkstra_res->path[currentEdgeIndex + 1];
                int weight = getEdgeWeight(graph, src, dst);

                if (timer >= 0.3f) {
                    timer = 0.0f;
                    currentStep++;

                    if (currentStep >= weight) {
                        currentStep = 0;
                        currentEdgeIndex++;

                        if (currentEdgeIndex >= dijkstra_res->pathLength - 1) {
                            finished = true;
                            isPlaying = false;
                        } else {
                            int arrivedNode = dijkstra_res->path[currentEdgeIndex];

                            if (arrivedNode != dijkstra_res->path[0] &&
                                arrivedNode != dijkstra_res->path[dijkstra_res->pathLength - 1]) {
                                isWaitingAtNode = true;
                            }
                        }
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(bgCol);

        DrawText("Traffic Simulation", 12, 12, 20, (Color){160,160,180,255});

        drawEdges(graph, pos, has_edge);
        drawVertices(graph->numOfVertices, pos);

        DrawRectangleRec(playButton, isPlaying ? RED : GREEN);
        DrawRectangleLinesEx(playButton, 2, BLACK);

        if (isPlaying) {
            DrawText("STOP", 55, 55, 20, WHITE);
        } else {
            DrawText("PLAY", 55, 55, 20, WHITE);
        }

        if (dijkstra_res->pathLength > 0) {
            Vector2 entityPos;

            if (finished || dijkstra_res->pathLength == 1) {
                int lastNode = dijkstra_res->path[dijkstra_res->pathLength - 1];
                entityPos = pos[lastNode];
            } else {
                int src = dijkstra_res->path[currentEdgeIndex];
                int dst = dijkstra_res->path[currentEdgeIndex + 1];
                int weight = getEdgeWeight(graph, src, dst);

                entityPos = getEntityPosition(pos[src], pos[dst], currentStep, weight);
            }

            DrawCircleV(entityPos, 10, ORANGE);
            DrawCircleLinesV(entityPos, 10, BLACK);
        }

        if (isWaitingAtNode) {
            DrawText("Waiting at node...", 20, 95, 20, YELLOW);
        }

        if (finished) {
            DrawText("Arrived at destination!", 20, 95, 24, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
}