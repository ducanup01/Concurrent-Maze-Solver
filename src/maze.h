#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

typedef struct Maze Maze;
typedef struct Cell Cell;

Maze* generateMaze(int rows, int cols);
void freeMaze(Maze *m);
void printMaze(Maze *m);

Cell *maze_get_start(Maze *m);
int maze_get_neighbors(Maze *m, Cell *c, Cell *out[4]);
bool maze_is_end(Maze *m, Cell *c);

static void shuffle(Cell **arr, int n);
static void removeWall(Cell *a, Cell *b);

#endif