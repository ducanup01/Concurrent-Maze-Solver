#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "maze.h"
#include "linkedListQueue.h"

extern int number_of_threads_input;
#define TIME_DELAY_PER_THREAD 200000

volatile bool hasExitBeenFound = false;

Maze* myMaze;

pthread_mutex_t mutexQueue;

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

        Cell* mycell = dequeue(queueOfCell);
        if (mycell == NULL) {
            pthread_mutex_unlock(&mutexQueue);
            continue;
        }

        if (mycell->visited) {
            pthread_mutex_unlock(&mutexQueue);
            continue;
        }

        mycell->visited = true;
        mycell->visitedBy = thread_id;
        mycell->content = 'v';

        if (mycell->isEnd) {
            hasExitBeenFound = true;
            pthread_mutex_unlock(&mutexQueue);
            printMaze(myMaze);
            break;
        }

        if (mycell->up && !mycell->up->visited)
            enqueue(mycell->up, queueOfCell);

        if (mycell->down && !mycell->down->visited)
            enqueue(mycell->down, queueOfCell);

        if (mycell->left && !mycell->left->visited)
            enqueue(mycell->left, queueOfCell);

        if (mycell->right && !mycell->right->visited)
            enqueue(mycell->right, queueOfCell);

        printMaze(myMaze);
        pthread_mutex_unlock(&mutexQueue);
        usleep(TIME_DELAY_PER_THREAD);
    }
}


void solveMazeConcurrently(Maze* maze)
{
    Cell* start = maze->start;
    queueOfCell = createQueue();
    
    pthread_mutex_init(&mutexQueue, NULL);

    enqueue(start, queueOfCell);


    pthread_t threadArray[number_of_threads_input];
    int threadIDs[number_of_threads_input];

    for (int i = 0; i < number_of_threads_input; i++) {
        threadIDs[i] = i;
        if (pthread_create(&threadArray[i], NULL, &processNodeTask, &threadIDs[i]) != 0)
            perror("Failed to create thread");
    }

    for (int i = 0; i < number_of_threads_input; i++)
    {
        if (pthread_join(threadArray[i], NULL) != 0)
        perror("Fail to join thread");
    }

    pthread_mutex_destroy(&mutexQueue);
}