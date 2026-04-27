#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "maze.h"
#include "mazeRunner.h"
#include "stack.h"
#include <time.h>

int number_of_threads_input;
int input_number_of_rows;
int input_number_of_cols;

extern Maze* myMaze;

int main()
{
    initThreadColors();
    
    printf("Choose number of rows: ");
    scanf("%d", &input_number_of_rows);

    printf("Choose number of columns: ");
    scanf("%d", &input_number_of_cols);

    myMaze = generateMazeRandomPositions(input_number_of_rows, input_number_of_cols);
    // myMaze = generateMazeRandomPositions(40, 120);
    // myMaze = generateImperfectMazeRandomPositions(6, 30);
    // myMaze = loadMazeBinary("../saved_mazes/maze6x15.bin");
    if (!myMaze) {
        printf("Failed to load maze\n");
        exit(1);
    }

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

    printf("Choose number of threads (1-9): ");
    scanf("%d", &number_of_threads_input);

    solveMazeConcurrently(myMaze);
    
    // saveMazeBinary(myMaze, "maze6x15.bin");
    freeMaze(myMaze);

    return 0;
}