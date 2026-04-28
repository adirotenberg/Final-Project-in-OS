#ifndef PROJECTOS_HEAP_H
#define PROJECTOS_HEAP_H


typedef struct {
    int node;
    int dist;
} HeapNode;

typedef struct {
    HeapNode *arr;
    int size;
    int capacity;
} MinHeap;

MinHeap* createHeap(int capacity);
void swap(HeapNode *a, HeapNode *b);
void heapifyUp(MinHeap *h, int i);
void heapifyDown(MinHeap *h, int i);
void insert(MinHeap *h, int node, int dist);
HeapNode extractMin(MinHeap *h);
void freeHeap(MinHeap *h);

#endif //PROJECTOS_HEAP_H