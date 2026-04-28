#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "maze.h"
#include "mazeRunner.h"
#include "stack.h"
#include <time.h>
#include "menu.h"

int number_of_threads_input;
int input_number_of_rows;
int input_number_of_cols;

extern Maze* myMaze;

int main()
{
    
    initThreadColors();

    int choice = runMenu();
    printf("\033[H\033[J");

    switch (choice)
    {
    case 0: // Randomize maze
        printf("Enter rows: ");
        scanf("%d", &input_number_of_rows);
        printf("Enter cols: ");
        scanf("%d", &input_number_of_cols);
        myMaze = generateMazeRandomPositions(input_number_of_rows, input_number_of_cols);
        break;

    case 1: // Randomize imperfect maze
        printf("Enter rows: ");
        scanf("%d", &input_number_of_rows);
        printf("Enter cols: ");
        scanf("%d", &input_number_of_cols);
        myMaze = generateImperfectMazeRandomPositions(input_number_of_rows, input_number_of_cols);
        break;

    case 2:
        myMaze = loadMazeBinary("../saved_mazes/maze10x15.bin");
        break;

    case 3:
        myMaze = loadMazeBinary("../saved_mazes/maze15x25.bin");
        break;

    case 4:
        myMaze = loadMazeBinary("../saved_mazes/maze18x30.bin");
        break;

    case 5:
        myMaze = loadMazeBinary("../saved_mazes/maze22x35.bin");
        break;

    case 6:
        myMaze = loadMazeBinary("../saved_mazes/maze25x45.bin");
        break;

    case 7:
        myMaze = loadMazeBinary("../saved_mazes/maze30x55.bin");
        break;

    case 8:
        myMaze = loadMazeBinary("../saved_mazes/maze35x65.bin");
        break;

    case 9:
        myMaze = loadMazeBinary("../saved_mazes/maze40x75.bin");
        break;

    case 10:
        myMaze = loadMazeBinary("../saved_mazes/maze50x85.bin");
        break;

    case 11:
        exit(0);
    }

    if (!myMaze) {
        printf("Failed to load maze\n");
        exit(1);
    }

    struct timespec start, end;

    printMaze(myMaze);

    printf("Choose number of threads (1-100): ");
    scanf("%d", &number_of_threads_input);

    clock_gettime(CLOCK_MONOTONIC, &start);
    solveMazeConcurrently(myMaze);
    clock_gettime(CLOCK_MONOTONIC, &end);

    displayShortestPath(myMaze);

    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    long total_ns = seconds * 1000000000L + nanoseconds;

    long s = total_ns / 1000000000L;
    long ms = (total_ns % 1000000000L) / 1000000L;
    long us = (total_ns % 1000000L) / 1000L;
    long ns = total_ns % 1000L;

    printf("\nTime taken: %lds %ldms %ldus %ldns\n", s, ms, us, ns);

    printf("Distance from start: %d tiles\n", myMaze->end->distanceFromStart);

    // saveMazeBinary(myMaze, "maze50x85.bin");
    freeMaze(myMaze);

    return 0;
}