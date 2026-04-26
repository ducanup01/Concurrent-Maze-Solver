#ifndef QUEUE
#define QUEUE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct LinkedList LinkedList;

typedef struct Queue Queue;

void printQueue(Queue *q);

Queue *createQueue();

bool isEmpty(Queue *q);

void enqueue(int data, Queue *q);

int dequeue(Queue *q);

#endif