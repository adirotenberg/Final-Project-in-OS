#include "MinHeap.h"
#include <stdlib.h>


MinHeap *createHeap(int capacity) {
    MinHeap *h = malloc(sizeof(MinHeap));
    h->arr = malloc(sizeof(HeapNode) * capacity);
    h->size = 0;
    h->capacity = capacity;
    return h;
}

void swap(HeapNode *a, HeapNode *b) {
    HeapNode tmp = *a;
    *a = *b;
    *b = tmp;
}

void heapifyUp(MinHeap *h, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;

        if (h->arr[parent].dist <= h->arr[i].dist)
            break;

        swap(&h->arr[parent], &h->arr[i]);
        i = parent;
    }
}

void heapifyDown(MinHeap *h, int i) {
    int smallest = i;

    while (1) {
        int left = 2*i + 1;
        int right = 2*i + 2;

        if (left < h->size && h->arr[left].dist < h->arr[smallest].dist)
            smallest = left;

        if (right < h->size && h->arr[right].dist < h->arr[smallest].dist)
            smallest = right;

        if (smallest == i)
            break;

        swap(&h->arr[i], &h->arr[smallest]);
        i = smallest;
    }
}

void insert(MinHeap *h, int node, int dist) {
    h->arr[h->size].node = node;
    h->arr[h->size].dist = dist;
    heapifyUp(h, h->size);
    h->size++;
}

HeapNode extractMin(MinHeap *h) {
    HeapNode root = h->arr[0];
    h->arr[0] = h->arr[h->size - 1];
    h->size--;
    heapifyDown(h, 0);
    return root;
}

void freeHeap(MinHeap *h) {
    if (h == NULL) return;

    free(h->arr);
    free(h);
}