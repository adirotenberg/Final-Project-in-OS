#define _POSIX_C_SOURCE 200809L //in order for kill to work in c11
#include "simulation.h"
#include "vizGraph.h"

#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


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
    float t = (float) step / (float) totalSteps;

    Vector2 pos;
    pos.x = from.x + (to.x - from.x) * t;
    pos.y = from.y + (to.y - from.y) * t;

    return pos;
}

static Color getRandomColor(int index) {
    Color colors[] = {
        RED, GREEN, ORANGE, PURPLE, PINK, LIME, GOLD, MAROON, BEIGE, MAGENTA
    };
    return colors[index % (sizeof(colors) / sizeof(Color))];
}

void simulation(InputData* data, int pipes[][2], pid_t* pids, int numOfTravelers){

    Graph *graph = data->graph;

    Vector2 pos[MAX_VERTICES];
    bool has_edge[MAX_VERTICES][MAX_VERTICES] = {0};

    layout(graph, pos, has_edge);

    TravelerState *states = malloc(sizeof(TravelerState) * numOfTravelers);
    if (states == NULL) {
        printf("Error: Failed to create traveler states\n");
        return;
    }
    for (int i = 0; i < numOfTravelers; i++) {
        states[i].currentEdgeIndex = 0;
        states[i].currentStep = 0;
        states[i].timer = 0.0f;
        states[i].waitTimer = 0.0f;
        states[i].isWaitingAtNode = false;
        states[i].finished = false;
        states[i].color = getRandomColor(i);
        states[i].signalSent = false;
        states[i].currentNode = data->travelers[i][0];
        states[i].nextNode = -1;
    }

    bool isPlaying = false;
    bool allFinished = false;

    Rectangle playButton = {20, 45, 120, 40};

    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(SCREEN_W, SCREEN_H, "Traffic Simulation - Milestone 3");
    Texture2D worldMap = LoadTexture("assets/world_map.png");
    Texture2D plane = LoadTexture("assets/plane.png");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();

            if (CheckCollisionPointRec(mouse, playButton)) {
                isPlaying = !isPlaying;
            }
        }


        if (isPlaying && !allFinished) {

            for (int i = 0; i < numOfTravelers; i++) {

                // if plane is free read first from pipe
                if (states[i].finished || states[i].nextNode != -1 || states[i].isWaitingAtNode) {
                    continue;
                }

                TravelMessage msg;
                ssize_t bytesRead = read(pipes[i][0], &msg, sizeof(TravelMessage));

                if (bytesRead == sizeof(TravelMessage)) {
                    states[i].currentNode = msg.currentNode;
                    states[i].nextNode = msg.nextNode;
                    states[i].finished = msg.finished;
                    states[i].currentStep = 0;

                    if (msg.finished) {
                        printf("[PID=%d] arrived at node %d | DESTINATION\n",
                               msg.pid, msg.currentNode);
                        printf("[PID=%d] finished\n", msg.pid);
                    } else {
                        printf("[PID=%d] arrived at node %d | next node: %d\n",
                               msg.pid, msg.currentNode, msg.nextNode);
                    }
                }
            }

            allFinished = true;

            // moving the planes
            for (int i = 0; i < numOfTravelers; i++) {

                if (states[i].finished) {
                    continue;
                }

                allFinished = false;

                if (states[i].isWaitingAtNode) {
                    states[i].waitTimer += dt;

                    if (states[i].waitTimer >= 1.0f) {
                        states[i].waitTimer = 0.0f;
                        states[i].isWaitingAtNode = false;
                    }

                    continue;
                }

                if (states[i].nextNode == -1) {
                    continue;
                }

                states[i].timer += dt;

                int src = states[i].currentNode;
                int dst = states[i].nextNode;
                int weight = getEdgeWeight(graph, src, dst);

                if (states[i].timer >= 0.3f) {
                    states[i].timer = 0.0f;
                    states[i].currentStep++;

                    if (states[i].currentStep >= weight) {
                        states[i].currentNode = states[i].nextNode;
                        states[i].nextNode = -1;
                        states[i].currentStep = 0;
                        states[i].isWaitingAtNode = true;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(simBgCol);

        DrawTexturePro(
            worldMap,
            (Rectangle){0, 0, worldMap.width, worldMap.height},
            (Rectangle){0, 0, SCREEN_W, SCREEN_H},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 70});

        DrawText("Traffic Simulation", 12, 12, 20, (Color){160, 160, 180, 255});

        drawEdges(graph, pos, has_edge);
        drawVertices(graph->numOfVertices, pos);

        DrawRectangleRec(playButton, isPlaying ? RED : GREEN);
        DrawRectangleLinesEx(playButton, 2, BLACK);

        if (isPlaying) {
            DrawText("STOP", 55, 55, 20, WHITE);
        } else {
            DrawText("PLAY", 55, 55, 20, WHITE);
        }

        for (int i = 0; i < numOfTravelers; i++) {
            Vector2 entityPos;

            if (states[i].finished || states[i].nextNode == -1) {
                entityPos = pos[states[i].currentNode];
            } else {
                int src = states[i].currentNode;
                int dst = states[i].nextNode;
                int weight = getEdgeWeight(graph, src, dst);

                entityPos = getEntityPosition(
                        pos[src],
                        pos[dst],
                        states[i].currentStep,
                        weight
                );
            }

            if (!states[i].finished && states[i].nextNode != -1) {
                DrawLineEx(
                        pos[states[i].currentNode],
                        pos[states[i].nextNode],
                        5.0f,
                        states[i].color
                );
            }

            DrawTexturePro(
                    plane,
                    (Rectangle){0, 0, plane.width, plane.height},
                    (Rectangle){entityPos.x, entityPos.y, 42, 42},
                    (Vector2){21, 21},
                    0.0f,
                    states[i].color
            );
        }

        if (allFinished) {
            DrawText("All travelers arrived!", 20, 95, 24, GREEN);
        }

        EndDrawing();
    }

    UnloadTexture(worldMap);
    UnloadTexture(plane);

    free(states);
    CloseWindow();
}
