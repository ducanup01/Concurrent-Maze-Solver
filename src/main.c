#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "maze.h"

#include "stack.h"

void main()
{
    // Maze *myMaze = generateMaze(45, 120);
    Maze *myMaze = loadMazeBinary("maze10x20.bin");

    Node *startingPoint = buildGraph(myMaze);

    printMaze(myMaze);

    // check right node
    printf("%d\n", startingPoint->right->visited);

    // don't do this:
    // printf("%d\n", startingPoint->down->visited);

    // saveMazeBinary(myMaze, "maze45x120.bin");

    freeMaze(myMaze);

}