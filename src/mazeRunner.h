#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "maze.h"
#include "linkedListQueue.h"

extern int number_of_threads_input;
#define TIME_DELAY_PER_THREAD 180000

volatile bool hasExitBeenFound = false;

Maze* myMaze;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

Queue* queueOfCell;

void* processNodeTask(void* args)
{
    int thread_id = *(int*) args;
    while (1)
    {
        pthread_mutex_lock(&mutexQueue);
        if (hasExitBeenFound) {
            pthread_mutex_unlock(&mutexQueue);
            break;
        }

        // Cell* mycell = dequeue(queueOfCell);
        // if (mycell == NULL) {
        //     pthread_mutex_unlock(&mutexQueue);
        //     continue;
        // }

        while (isQueueEmpty(queueOfCell)) {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        Cell* mycell = dequeue(queueOfCell);

        mycell->visited = true;
        mycell->visitedBy = thread_id;
        // mycell->content = 'v';

        if (mycell->isEnd) {
            hasExitBeenFound = true;
            printMaze(myMaze);
            pthread_mutex_unlock(&mutexQueue);
            break;
        }

        if (mycell->up && !mycell->up->visited) {
            mycell->up->distanceFromStart = mycell->distanceFromStart + 1;
            enqueue(mycell->up, queueOfCell);
        }

        if (mycell->down && !mycell->down->visited) {
            mycell->down->distanceFromStart = mycell->distanceFromStart + 1;
            enqueue(mycell->down, queueOfCell);
        }

        if (mycell->left && !mycell->left->visited) {
            mycell->left->distanceFromStart = mycell->distanceFromStart + 1;
            enqueue(mycell->left, queueOfCell);
        }

        if (mycell->right && !mycell->right->visited) {
            mycell->right->distanceFromStart = mycell->distanceFromStart + 1;
            enqueue(mycell->right, queueOfCell);
        }

        printMaze(myMaze);
        pthread_cond_broadcast(&condQueue);
        pthread_mutex_unlock(&mutexQueue);
        usleep(TIME_DELAY_PER_THREAD);
    }
}


void solveMazeConcurrently(Maze* maze)
{
    Cell* start = maze->start;
    queueOfCell = createQueue();
    
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);

    enqueue(start, queueOfCell);
    start->visited = true;
    start->distanceFromStart = 0;

    pthread_t threadArray[number_of_threads_input];
    int threadIDs[number_of_threads_input];

    for (int i = 0; i < number_of_threads_input; i++) {
        threadIDs[i] = i;
        if (pthread_create(&threadArray[i], NULL, &processNodeTask, &threadIDs[i]) != 0)
            perror("Failed to create thread");
    }

    for (int i = 0; i < number_of_threads_input; i++)
    {
        if (pthread_join(threadArray[i], NULL) != 0) perror("Fail to join thread");
    }

    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
}