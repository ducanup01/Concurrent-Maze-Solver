// #include "linkedListQueue.h"
#ifndef LINKEDLISTQUEUE
#define LINKEDLISTQUEUE
#include "maze.h"

typedef struct LinkedList {
    Cell* cell;
    struct LinkedList *next;
} LinkedList;

typedef struct {
    LinkedList *head;
    LinkedList *rear;
    int elementCount;
} Queue;

Queue* createQueue();

bool isQueueEmpty(Queue* q);

void enqueue(Cell* cell, Queue* q);

Cell* dequeue(Queue *q);

#endif