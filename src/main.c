#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "maze.h"

#include "stack.h"

void main()
{
    Maze *myMaze = generateMaze(10, 26);
    printMaze(myMaze);

    freeMaze(myMaze);

}