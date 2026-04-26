#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "linkedListQueue.h"

// typedef struct LinkedList {
//     int data;
//     struct LinkedList *next;
// } LinkedList;

// typedef struct {
//     LinkedList *head;
//     LinkedList *rear;
//     int elementCount;
// } Queue;

void printQueue(Queue* q)
{
    LinkedList *current = q->head;
    while (current != NULL)
    {
        printf("%d\n", current->data);
        current = current->next;
    }
}

Queue* createQueue()
{
    Queue* q = malloc(sizeof(Queue));
    q->head = NULL;
    q->rear = NULL;
    q->elementCount = 0;
    return q;
}

bool isQueueEmpty(Queue* q)
{
    if (q->elementCount == 0) return true;
    return false;
}

void enqueue(int data, Queue* q)
{
    LinkedList* newNode = malloc(sizeof(LinkedList));
    newNode->data = data;
    newNode->next = NULL;

    if (q->head == NULL)
    {
        q->head = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->elementCount++;
}

int dequeue(Queue* q)
{
    if (isEmpty(q)) return NULL;

    LinkedList* temp = q->head;
    int data = q->head->data;

    q->head = q->head->next;

    if (q->head == NULL) q->rear = NULL;

    q->elementCount--;

    free(temp);

    return data;

}