// #include "linkedListQueue.h"
#ifndef LINKEDLISTQUEUE
#define LINKEDLISTQUEUE

typedef struct LinkedList {
    int data;
    struct LinkedList *next;
} LinkedList;

typedef struct {
    LinkedList *head;
    LinkedList *rear;
    int elementCount;
} Queue;

void printQueue(Queue* q);

Queue* createQueue();

bool isQueueEmpty(Queue* q);

void enqueue(int data, Queue* q);

int dequeue(Queue* q);

#endif