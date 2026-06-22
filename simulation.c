#define _POSIX_C_SOURCE 200809L
#include "simulation.h"
#include "vizGraph.h"

#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int addToQueue(int i, int node, int schd, InputData* data, int ** waitingQueues, int * queuesLengths);
void leaveQueue(int i, int node, int ** waitingQueues, int * queuesLengths);

void simulation(InputData* data, int pipes[][2], int numOfTravelers, int schd, int ** waitingQueues, int * queuesLengths){
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
        states[i].edgeProgress = 0.0f;
        states[i].isWaitingAtNode = false;
        states[i].isQueueingOutside = false;
        states[i].finished = false;
        states[i].color = getRandomColor(i);
        states[i].signalSent = false;
        states[i].pid = 0;
        states[i].currentNode = data->travelers[i][0];
        states[i].nextNode = -1;
    }

    for (int v = 0; v < graph->numOfVertices; v++) {
        pthread_mutex_init(&(graph->vertices[v].node_mutex), NULL);
        graph->vertices[v].occupying_traveler_id = -1;
    }

    // Initial Node Entry: attempt to lock the starting node for everyone
    for (int i = 0; i < numOfTravelers; i++) {
        int startNode = states[i].currentNode;
        if (pthread_mutex_trylock(&(graph->vertices[startNode].node_mutex)) == 0) {
            // Success: Occupy the node and start the 1s dwell timer
            graph->vertices[startNode].occupying_traveler_id = i;
            states[i].isWaitingAtNode = true;
        } else {
            // Failure: Node is already occupied (someone else started there), wait outside
            states[i].isQueueingOutside = true;
            states[i].nextNode = startNode;
            addToQueue(i, startNode, schd, data, waitingQueues, queuesLengths);
        }
    }

    bool isPlaying = false;
    bool allFinished = false;

    Rectangle playButton = {20, 45, 120, 40};

    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    const char* title;
    if (schd == FCFS) {
        title = "Traffic Simulation - FCFS Scheduling";
    } else if (schd == PRIORITY) {
        title = "Traffic Simulation - Priority Scheduling";
    } else {
        title = "Traffic Simulation - Unknown Scheduling";
    }
    InitWindow(SCREEN_W, SCREEN_H, title);
    Texture2D worldMap = LoadTexture("assets/world_map.png");
    Texture2D plane = LoadTexture("assets/plane.png");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();

            if (CheckCollisionPointRec(mouse, playButton)) {
                isPlaying = !isPlaying;
                if (isPlaying) printf("Simulation started!\n");
            }
        }

        if (isPlaying && !allFinished) {
            //going through travelers to check if they sent a message about the next node they need to get to
            for (int i = 0; i < numOfTravelers; i++) {
                //if we already read that traveler's signal / it finished got to its destination / it's waiting at a node / waiting to get into a node (because it's occupied)
                //then we don't need to read a signal from that traveler
                if (states[i].signalSent || states[i].nextNode != -1 || states[i].isWaitingAtNode || states[i].isQueueingOutside) {
                    continue;
                }

                TravelMessage msg;
                ssize_t bytesRead = read(pipes[i][0], &msg, sizeof(TravelMessage));

                if (bytesRead == sizeof(TravelMessage)) {
                    states[i].currentStep = 0;
                    states[i].edgeProgress = 0.0f;
                    states[i].pid = msg.pid;

                    if (msg.finished) {
                        // Check if we are already at the destination node
                        if (msg.currentNode == states[i].currentNode && states[i].nextNode == -1) {
                            states[i].finished = true;
                            // RELEASE IMMEDIATELY because we reached the destination
                            int nodeToRelease = states[i].currentNode;
                            graph->vertices[nodeToRelease].occupying_traveler_id = -1;
                            pthread_mutex_unlock(&(graph->vertices[nodeToRelease].node_mutex));
                            states[i].signalSent = true;
                            printf("Traveler %d finished at destination node %d and released it.\n", i, nodeToRelease);
                        } else {
                            states[i].nextNode = msg.currentNode;
                            states[i].finished = true;
                            states[i].isQueueingOutside = true;
                        }
                    } else {
                        states[i].nextNode = msg.nextNode;
                    }
                }
            }

            allFinished = true;

            for (int i = 0; i < numOfTravelers; i++) {

                if (states[i].signalSent) {
                    continue;
                }

                allFinished = false;

                //if the traveler is waiting outside for the node, it's going to try to get in as long as the mutex isn't locked (meaning, the node isn't occupied)
                //but only the first traveler in the waiting queue for that node will try to get in
                int targetNode = states[i].nextNode;
                if (states[i].isQueueingOutside && waitingQueues[targetNode][0] == i) {
                    if (pthread_mutex_trylock(&(graph->vertices[targetNode].node_mutex)) == 0) {
                        graph->vertices[targetNode].occupying_traveler_id = i;
                        states[i].isQueueingOutside = false;
                        leaveQueue(i, targetNode,waitingQueues,queuesLengths);

                        if (states[i].currentNode != targetNode) {
                            states[i].currentNode = targetNode;
                        }
                        states[i].nextNode = -1;
                        states[i].currentStep = 0;
                        states[i].edgeProgress = 0.0f;

                        if (states[i].finished) {
                            // If this is the final destination, release immediately and vanish
                            graph->vertices[targetNode].occupying_traveler_id = -1;
                            pthread_mutex_unlock(&(graph->vertices[targetNode].node_mutex));
                            states[i].signalSent = true;
                            printf("Traveler %d reached final destination and left the system.\n", i);
                        } else {
                            // Intermediate node: must wait for 1 second
                            states[i].isWaitingAtNode = true;
                            printf("Traveler %d acquired the lock for Node %d and entered.\n", i, targetNode);
                        }
                    }
                    continue;
                }

                // Traveler is queueing outside but is not first in the queue — just wait
                if (states[i].isQueueingOutside) {
                    continue;
                }

                if (states[i].isWaitingAtNode) {
                    states[i].waitTimer += dt;

                    if (states[i].waitTimer >= 1.0f) {
                        states[i].waitTimer = 0.0f;
                        states[i].isWaitingAtNode = false;

                        // Only release and vanish if finished.
                        // If not finished, we keep the lock until we start moving to the next node.
                        if (states[i].finished) {
                            int nodeToRelease = states[i].currentNode;
                            graph->vertices[nodeToRelease].occupying_traveler_id = -1;
                            pthread_mutex_unlock(&(graph->vertices[nodeToRelease].node_mutex));

                            states[i].signalSent = true;
                            printf("Traveler %d finished and left the system.\n", i);
                        }
                    }

                    continue;
                }

                if (states[i].nextNode == -1) {
                    continue;
                }

                int src = states[i].currentNode;
                int dst = states[i].nextNode;
                int weight = getEdgeWeight(graph, src, dst);

                // Smooth movement based on edge weight
                float travelDuration = weight * 0.3f;
                states[i].edgeProgress += dt / travelDuration;

                // Release the previous node lock once we start moving
                if (states[i].edgeProgress > 0.0f && states[i].edgeProgress < 1.0f) {
                    if (graph->vertices[src].occupying_traveler_id == i) {
                        graph->vertices[src].occupying_traveler_id = -1;
                        pthread_mutex_unlock(&(graph->vertices[src].node_mutex));
                    }
                }

                // Find if traveler is already in the queue
                int queuePos = -1;
                for (int q = 0; q < queuesLengths[dst]; q++) {
                    if (waitingQueues[dst][q] == i) {
                        queuePos = q;
                        break;
                    }
                }

                if (queuePos == -1) {
                    // Not in queue yet. Check if we reached the end of the current queue.
                    int N = queuesLengths[dst];
                    float triggerProgress = 0.9f - 0.08f * N;
                    if (triggerProgress < 0.1f) triggerProgress = 0.1f;

                    if (states[i].edgeProgress >= triggerProgress) {
                        // Join the queue
                        int placeInQueue = addToQueue(i, dst, schd, data, waitingQueues, queuesLengths);
                        printf("[PID=%d] Approaching Node %d. Waiting for entry permission...\n", states[i].pid, dst);
                        
                        // Update queuePos to see where we were placed
                        queuePos = placeInQueue;
                    }
                }

                if (queuePos != -1) {
                    float targetProgress = 0.9f - 0.08f * queuePos;
                    if (targetProgress < 0.1f) targetProgress = 0.1f;

                    if (states[i].edgeProgress >= targetProgress) {
                        states[i].edgeProgress = targetProgress;
                        states[i].isQueueingOutside = true;
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

        DrawText(title, 12, 12, 20, (Color){160, 160, 180, 255});

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
            if (states[i].signalSent) {
                continue;
            }
            Vector2 entityPos;

            // Priority: If queueing, ALWAYS show outside
            if (states[i].isQueueingOutside) {
                int src = states[i].currentNode;
                int dst = states[i].nextNode;
                Vector2 from = pos[src];
                Vector2 to = pos[dst];

                int queuePos = -1;
                for (int q = 0; q < queuesLengths[dst]; q++) {
                    if (waitingQueues[dst][q] == i) {
                        queuePos = q;
                        break;
                    }
                }
                if (queuePos == -1) {
                    queuePos = 0;
                }
                if (src == dst) {
                    entityPos = (Vector2){ to.x - 35 * (queuePos + 1), to.y - 35 * (queuePos + 1) };
                } else {
                    float t = 0.9f - 0.08f * queuePos;
                    if (t < 0.1f) t = 0.1f;
                    entityPos.x = from.x + (to.x - from.x) * t;
                    entityPos.y = from.y + (to.y - from.y) * t;
                }
            }
            // Else if waiting inside or stationary at destination
            else if (states[i].isWaitingAtNode || states[i].finished || states[i].nextNode == -1) {
                entityPos = pos[states[i].currentNode];
            }
            // Else gliding along an edge
            else {
                int src = states[i].currentNode;
                int dst = states[i].nextNode;
                Vector2 from = pos[src];
                Vector2 to = pos[dst];
                entityPos.x = from.x + (to.x - from.x) * states[i].edgeProgress;
                entityPos.y = from.y + (to.y - from.y) * states[i].edgeProgress;
            }

            Color drawColor = states[i].color;
            if (states[i].isQueueingOutside) {
                drawColor = YELLOW;
                DrawText("WAITING", (int)entityPos.x - 20, (int)entityPos.y - 35, 12, YELLOW);
            }

            DrawTexturePro(
                    plane,
                    (Rectangle){0, 0, plane.width, plane.height},
                    (Rectangle){entityPos.x, entityPos.y, 42, 42},
                    (Vector2){21, 21},
                    0.0f,
                    drawColor
            );
            DrawCircle((int)entityPos.x, (int)entityPos.y, 10, drawColor);
            char str[12];
            snprintf(str, sizeof(str), "%d", data->travelers[i][2]);
            DrawText(str, (int)entityPos.x - 5, (int)entityPos.y - 10, 20, BLACK);
        }

        if (allFinished) {
            DrawText("All travelers arrived!", 20, 95, 24, GREEN);
        }

        EndDrawing();
    }

    for (int v = 0; v < graph->numOfVertices; v++) {
        pthread_mutex_destroy(&(graph->vertices[v].node_mutex));
    }

    UnloadTexture(worldMap);
    UnloadTexture(plane);

    free(states);
    CloseWindow();
}

int addToQueue(int i, int node, int schd, InputData* data, int ** waitingQueues, int * queuesLengths) {
    //if we're trying to add a traveler to a queue that's already full, we're not going to let it (even though this situation can't actually happen, we only have numOfTravelers amount of travelers
    if (queuesLengths[node] == data->numOfTravelers) return -1;

    //if the scheduling algorithm is FCFS, (or it's Priority but the queue for that node is empty anyway)
    //just add the traveler to the first unoccupied place in the queue (or in Priority case, to the beginning of the queue)
    if (schd == FCFS || (schd == PRIORITY && queuesLengths[node] == 0)) {
        waitingQueues[node][queuesLengths[node]++] = i;
        return queuesLengths[node] - 1;
    }
    //else, the scheduling is Priority and we need to add the traveler to the queue according to its priority
    int idx = queuesLengths[node] - 1;

    // Walk backwards, shifting elements to the right to make room
    while (idx >= 0) {
        // 1. Get the ID of the traveler currently sitting at this spot in the queue
        int travelerInQueue = waitingQueues[node][idx];

        // 2. Check priorities.
        // If the traveler in the queue is LESS important (higher priority number)
        // than our new traveler 'i', we shift them to the right.
        //the priority number is in the 3rd field of each traveler in the travelers array
        if (data->travelers[travelerInQueue][2] > data->travelers[i][2]) {
            waitingQueues[node][idx + 1] = travelerInQueue;
            idx--;
        } else {
            // We found someone more important or equal; stop shifting!
            break;
        }
    }

    // Drop the new element into its rightful spot
    waitingQueues[node][idx + 1] = i;
    queuesLengths[node]++;
    return idx + 1;

}

void leaveQueue(int i, int node, int ** waitingQueues, int * queuesLengths) {
    //if we accidentally try to get a different traveler from the queue that isn't the first one in the queue
    if (waitingQueues[node][0] != i) return;
    int idx = 1;

    //shifting elements to the left because the first element left the queue
    while (idx < queuesLengths[node]) {
        waitingQueues[node][idx - 1] = waitingQueues[node][idx];
        idx ++;
    }
    queuesLengths[node] --;
}