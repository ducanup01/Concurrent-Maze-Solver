#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "maze.h"
#include "mazeRunner.h"
#include "stack.h"
#include <time.h>

void main()
{

    // Maze *myMaze = generateMaze(8000, 8000);

    Maze *myMaze = loadMazeBinary("../saved_mazes/maze1Kx1K.bin");
    Node* startingPoint = buildGraph(myMaze);

    if (!myMaze)
    {
        printf("Failed to load maze\n");
        exit(1);
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    mazeTraverse(startingPoint);
    clock_gettime(CLOCK_MONOTONIC, &end);

    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    long total_ns = seconds * 1000000000L + nanoseconds;
    printf("Time taken: %ld\n", total_ns);

    // printMaze(myMaze);

    // don't do this:
    // printf("%d\n", startingPoint->down->visited);

    // saveMazeBinary(myMaze, "maze8Kx8K.bin");

    freeMaze(myMaze);

}