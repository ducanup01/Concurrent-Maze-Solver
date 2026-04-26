#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "maze.h"
#include "mazeRunner.h"
#include "stack.h"
#include <time.h>

int main()
{
    // Maze *myMaze = generateMazeRandomPositions(6, 15);
    // Maze *myMaze = generateImperfectMazeRandomPositions(6, 15);
    Maze *myMaze = loadMazeBinary("../saved_mazes/maze6x15.bin");
    if (!myMaze) {
        printf("Failed to load maze\n");
        exit(1);
    }
    
    Cell* startingPoint = myMaze->start;

    // struct timespec start, end;
    // clock_gettime(CLOCK_MONOTONIC, &start);

    // mazeTraverseRecursive(startingPoint);
    // mazeTraverseMultithreaded(startingPoint);

    // clock_gettime(CLOCK_MONOTONIC, &end);

    // long seconds = end.tv_sec - start.tv_sec;
    // long nanoseconds = end.tv_nsec - start.tv_nsec;
    // long total_ns = seconds * 1000000000L + nanoseconds;
    // printf("Time taken: %ld\n", total_ns);

    printMaze(myMaze);

    // saveMazeBinary(myMaze, "maze6x15.bin");

    solveMazeConcurrently(startingPoint);

    freeMaze(myMaze);

    return 0;
}