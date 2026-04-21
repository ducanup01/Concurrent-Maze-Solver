#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "maze.h"
#include "mazeRunner.h"
#include "stack.h"

void main()
{
    // Maze *myMaze = generateMaze(45, 120);
    Maze *myMaze = loadMazeBinary("../saved_mazes/maze45x120.bin");
    if (!myMaze)
    {
        printf("Failed to load maze\n");
        exit(1);
    }

    Node* startingPoint = buildGraph(myMaze);

    mazeTraverse(startingPoint);

    // printMaze(myMaze);

    // don't do this:
    // printf("%d\n", startingPoint->down->visited);

    // saveMazeBinary(myMaze, "maze45x120.bin");

    freeMaze(myMaze);

}