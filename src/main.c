#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "maze.h"
#include "mazeRunner.h"
#include "stack.h"
#include <time.h>
#include "menu.h"
#include <string.h>

int number_of_threads_input;
int input_number_of_rows;
int input_number_of_cols;

extern Maze* myMaze;

// Helper function to save custom mazes with user-defined names
void promptToSaveCustomMaze(Maze *m)
{
    char response;
    printf("\nSave this maze? (y/n): ");
    scanf(" %c", &response);
    
    if (response == 'y' || response == 'Y') {
        char mazeName[64];
        printf("Enter maze name (no spaces, e.g., 'my_maze'): ");
        scanf("%63s", mazeName);
        
        // Validate name
        if (strlen(mazeName) == 0) {
            printf("Invalid name. Skipping save.\n");
            return;
        }
        
        char filepath[256];
        snprintf(filepath, sizeof(filepath), "src/saved_mazes/%s.bin", mazeName);
        
        saveMazeBinary(m, filepath);
        printf("✓ Maze saved as '%s.bin' - will appear in menu next time you run!\n", mazeName);
    }
}

int main()
{
    
    initThreadColors();

    /* 
    CUSTOM MAZE FEATURE:
    - When you generate a random maze (options 0 or 1), you can save it with a custom name
    - Saved mazes are stored in src/saved_mazes/ as .bin files
    - These will automatically appear in the menu next time you run the program!
    */

    MenuResult result = runDynamicMenu();
    int choice = result.choice;
    printf("\033[H\033[J");

    // Handle menu choices
    if (choice == 0) {
        // Randomize maze
        printf("Enter rows: ");
        scanf("%d", &input_number_of_rows);
        printf("Enter cols: ");
        scanf("%d", &input_number_of_cols);
        myMaze = generateMazeRandomPositions(input_number_of_rows, input_number_of_cols);
        if (myMaze) {
            printf("\033[H\033[J");
            printMaze(myMaze);
            promptToSaveCustomMaze(myMaze);
        }
    }
    else if (choice == 1) {
        // Randomize imperfect maze
        printf("Enter rows: ");
        scanf("%d", &input_number_of_rows);
        printf("Enter cols: ");
        scanf("%d", &input_number_of_cols);
        myMaze = generateImperfectMazeRandomPositions(input_number_of_rows, input_number_of_cols);
        if (myMaze) {
            printf("\033[H\033[J");
            printMaze(myMaze);
            promptToSaveCustomMaze(myMaze);
        }
    }
    else if (choice == result.totalOptions - 1) {
        // Exit (last option)
        exit(0);
    }
    else if (choice >= 2 && choice < result.totalOptions - 1) {
        // Load a saved maze
        myMaze = loadMazeBinary(result.mazeFile);
        if (myMaze) {
            printf("\033[H\033[J");
            printMaze(myMaze);
        }
    }

    if (!myMaze) {
        printf("Failed to load maze\n");
        exit(1);
    }

    struct timespec start, end;

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
    
    // Clean up dynamically allocated menu filename
    if (result.mazeFile != NULL) {
        free(result.mazeFile);
    }

    return 0;
}