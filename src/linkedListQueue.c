#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "linkedListQueue.h"

Queue* createQueue()
{
    Queue* q = malloc(sizeof(Queue));
    if (!q) return NULL;

    q->head = NULL;
    q->rear = NULL;
    q->elementCount = 0;
    return q;
}

bool isQueueEmpty(Queue* q)
{
    return q->elementCount == 0;
}

void enqueue(Cell* cell, Queue *q)
{
    LinkedList *newNode = malloc(sizeof(LinkedList));
    if (!newNode) perror("Malloc failed");

    newNode->cell = cell;
    newNode->next = NULL;

    if (q->head == NULL)
    {
        q->head = newNode;
        q->rear = newNode;
    }
    else
    {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->elementCount++;
}

Cell* dequeue(Queue *q)
{
    if (isQueueEmpty(q))
        return NULL;

    LinkedList *temp = q->head;
    Cell* cell = q->head->cell;

    q->head = q->head->next;

    if (q->head == NULL)
        q->rear = NULL;

    q->elementCount--;

    free(temp);

    return cell;
}