#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

typedef struct Maze Maze;
typedef struct Cell Cell;
typedef struct Node {
    struct Node *up;
    struct Node *down;
    struct Node *left;
    struct Node *right;

    bool isEndingNode;

    // don't use these 3
    int row, col;
    bool visited;
    bool isStartingNode;

} Node;

Node *buildGraph(Maze *m);

Maze *generateMaze(int rows, int cols);
void freeMaze(Maze *m);
void printMaze(Maze *m);

// Cell *maze_get_start(Maze *m);
// int maze_get_neighbors(Maze *m, Cell *c, Cell *out[4]);
// bool maze_is_end(Maze *m, Cell *c);
void saveMazeBinary(Maze *m, const char *filename);
Maze *loadMazeBinary(const char *filename);

static void shuffle(Cell **arr, int n);
static void removeWall(Cell *a, Cell *b);

#endif