#include "vizGraph.h"

/* ── colors ───────────────────────────────────────────────────────── */
Color bgCol      = (Color){ 18,  18,  24, 255 };   /* near-black        */
Color nodeCol    = (Color){ 49,  90, 180, 255 };   /* deep blue         */
Color nodeBorder = (Color){ 90, 140, 230, 255 };   /* lighter blue rim  */
Color edgeCol    = (Color){180, 180, 200, 255 };   /* soft grey arrows  */
Color weightCol  = (Color){255, 210,  80, 255 };   /* gold weights      */
Color labelCol   = WHITE;

void layout(Graph* graph, Vector2 pos[], bool has_edge[][MAX_VERTICES]) {
    int numVertices = graph->numOfVertices;
    /* ── layout: arrange vertices in a circle ─────────────────────────── */
    float cx = SCREEN_W * 0.5f, cy = SCREEN_H * 0.5f;
    /* Shrink radius for small graphs so they don't feel cramped */
    float layoutR = (numVertices <= 4)  ? 140.0f
                  : (numVertices <= 7)  ? 200.0f
                  : (numVertices <= 10) ? 250.0f : 290.0f;


    for (int i = 0; i < numVertices; i++) {
        float angle = (2.0f * PI * i / numVertices) - PI / 2.0f;
        pos[i].x = cx + layoutR * cosf(angle);
        pos[i].y = cy + layoutR * sinf(angle);
    }

    /* ── build a reverse-edge lookup to decide which edges need curving ── */
    /* has_edge[s][d] = true if edge s→d exists */
    for (int i = 0; i < numVertices; i++) {
        int src = graph->vertices[i].id;
        Edge* curr = graph->vertices[i].adj;

        while (curr != NULL) {
            has_edge[src][curr->dst] = true;
            curr = curr->next;
        }

    }
}

void drawEdges(Graph* graph, Vector2 pos[], bool has_edge[][MAX_VERTICES]) {
    for (int i = 0; i < graph->numOfVertices; i++) {
        int src = graph->vertices[i].id;
        Edge* curr = graph->vertices[i].adj;
        while (curr != NULL) {
            int dst = curr->dst;
            int weight = curr->weight;

            if (src == dst) {
                DrawSelfLoop(pos[src], weight, edgeCol, weightCol);
                continue;
            }

            /* Curve this edge if the reverse edge also exists, so the two arrows don't overlap.
                Both directions get curved in the same (left-normal) sense, which naturally separates them.
             */
            bool curved = has_edge[dst][src];
            DrawEdge(pos[src], pos[dst], weight, curved, edgeCol, weightCol);
            curr = curr->next;
        }
    }
}

void drawVertices(int numVertices, Vector2 pos[]) {
    for (int i = 0; i < numVertices; i++) {
        /* outer glow ring */
        DrawCircle((int)pos[i].x, (int)pos[i].y,
                   VERTEX_RADIUS + 3, (Color){90,140,230, 60});
        /* filled circle */
        DrawCircleV(pos[i], VERTEX_RADIUS, nodeCol);
        /* border */
        DrawCircleLinesV(pos[i], VERTEX_RADIUS, nodeBorder);

        /* vertex number — centred inside the circle */
        char label[8]; sprintf(label, "%d", i);
        int fw = MeasureText(label, 18);
        DrawText(label,
                 (int)(pos[i].x - fw * 0.5f),
                 (int)(pos[i].y - 9),
                 18, labelCol);
    }
}

void drawGraph(Graph* graph) {
    Vector2 pos[MAX_VERTICES];
    bool has_edge[MAX_VERTICES][MAX_VERTICES] = {0};

    layout(graph, pos, has_edge);

    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(SCREEN_W, SCREEN_H, "Weighted Directed Graph");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(bgCol);

        /* title */
        DrawText("Weighted Directed Graph", 12, 12, 20, (Color){160,160,180,255});

        drawEdges(graph, pos, has_edge);

        drawVertices(graph->numOfVertices, pos);

        EndDrawing();
    }

    CloseWindow();
}