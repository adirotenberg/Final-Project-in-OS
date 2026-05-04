#include "vizHelperFuncs.h"

/* Rotate vector (x,y) by angle θ */
static Vector2 rotate2(float x, float y, float theta) {
    return (Vector2){ x * cosf(theta) - y * sinf(theta),
                      x * sinf(theta) + y * cosf(theta) };
}

/* Draw a filled arrowhead at `tip` pointing in direction (ux,uy) */
static void DrawArrowHead(Vector2 tip, float ux, float uy, Color col) {
    Vector2 left  = { tip.x - ARROW_HEAD * rotate2(ux, uy,  ARROW_ANGLE).x,
                      tip.y - ARROW_HEAD * rotate2(ux, uy,  ARROW_ANGLE).y };
    Vector2 right = { tip.x - ARROW_HEAD * rotate2(ux, uy, -ARROW_ANGLE).x,
                      tip.y - ARROW_HEAD * rotate2(ux, uy, -ARROW_ANGLE).y };
    /* Raylib needs CCW winding */
    DrawTriangle(tip, left, right, col);
}

/*
 * Draw a straight or curved directed edge from `from` to `to`.
 *   curved = true  → bend the line sideways (used for bidirectional pairs)
 *   curved = false → straight line
 */
void DrawEdge(Vector2 from, Vector2 to, int weight, bool curved, Color lineCol, Color textCol) {
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 1.0f) return;

    float ux = dx / len, uy = dy / len;   /* unit vector along the edge */
    float px = -uy,      py =  ux;         /* perpendicular (left normal) */

    /* Points on the surface of the source/destination circles */
    Vector2 start = { from.x + ux * VERTEX_RADIUS, from.y + uy * VERTEX_RADIUS };
    Vector2 end   = { to.x   - ux * VERTEX_RADIUS, to.y   - uy * VERTEX_RADIUS };

    Vector2 tip;   /* where the arrowhead tip will sit */

    if (!curved) {
        /* ── straight edge ──────────────────────────────────────────── */
        DrawLineEx(start, end, 2.2f, lineCol);
        tip = end;

        /* weight label: midpoint + small perpendicular offset */
        float mx = (start.x + end.x) * 0.5f + px * 14.0f;
        float my = (start.y + end.y) * 0.5f + py * 14.0f;
        char buf[16]; sprintf(buf, "%d", weight);
        int tw = MeasureText(buf, 15);
        DrawText(buf, (int)(mx - tw * 0.5f), (int)(my - 8), 15, textCol);

    } else {
        /* ── curved edge (quadratic Bézier approximated with 40 segments) ─ */
        float mx = (from.x + to.x) * 0.5f + px * CURVE_OFFSET;
        float my = (from.y + to.y) * 0.5f + py * CURVE_OFFSET;

        /* Adjust start/end so the curve leaves/arrives at the circle edge */
        /* (we keep the original start/end — they're already on the circle) */

        const int SEG = 40;
        Vector2 prev = start;
        for (int k = 1; k <= SEG; k++) {
            float t  = k / (float)SEG;
            float it = 1.0f - t;
            /* Quadratic Bézier: B(t) = it²·start + 2·it·t·mid + t²·end */
            Vector2 cur = {
                it*it * start.x + 2*it*t * mx + t*t * end.x,
                it*it * start.y + 2*it*t * my + t*t * end.y
            };
            DrawLineEx(prev, cur, 2.2f, lineCol);
            prev = cur;
        }



        /* Arrowhead: use tangent direction at t=1 */
        {
            float t = 1.0f, dt = 1.0f / SEG;
            float t2 = t - dt, it2 = 1.0f - t2;
            Vector2 p2 = {
                it2*it2 * start.x + 2*it2*t2 * mx + t2*t2 * end.x,
                it2*it2 * start.y + 2*it2*t2 * my + t2*t2 * end.y
            };
            float tdx = end.x - p2.x, tdy = end.y - p2.y;
            float tlen = sqrtf(tdx*tdx + tdy*tdy);
            tip = end;
            ux = tdx / tlen; uy = tdy / tlen;
        }

        /* Weight label at the peak of the curve */
        char buf[16]; sprintf(buf, "%d", weight);
        int tw = MeasureText(buf, 15);
        /* Offset label outward from the curve peak */
        DrawText(buf, (int)(mx - tw * 0.5f), (int)(my - 8), 15, textCol);
    }

    DrawArrowHead(tip, ux, uy, lineCol);
}

/*
 * Draw a self-loop: a small circle drawn to the upper-right of the vertex,
 * with a small arrowhead on the circle and the weight beside it.
 */
void DrawSelfLoop(Vector2 pos, int weight, Color lineCol, Color textCol) {
    /* The loop circle is offset diagonally from the vertex */
    float ox = VERTEX_RADIUS * 0.85f;
    float oy = -VERTEX_RADIUS * 0.85f;
    Vector2 center = { pos.x + ox, pos.y + oy };

    DrawCircleLines((int)center.x, (int)center.y, SELF_LOOP_R, lineCol);

    /* Arrowhead at the bottom of the loop circle, pointing left */
    Vector2 arrowTip = { center.x, center.y + SELF_LOOP_R };
    DrawArrowHead(arrowTip, -1.0f, 0.0f, lineCol);

    /* Weight label */
    char buf[16]; sprintf(buf, "%d", weight);
    int tw = MeasureText(buf, 15);
    DrawText(buf, (int)(center.x - tw * 0.5f + SELF_LOOP_R + 4),
             (int)(center.y - 8), 15, textCol);
}