#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "maze.h"

#include "stack.h"

void main()
{
    Maze *myMaze = generateMaze(6, 15);
    Node *startingPoint = buildGraph(myMaze);

    printMaze(myMaze);

    freeMaze(myMaze);

}