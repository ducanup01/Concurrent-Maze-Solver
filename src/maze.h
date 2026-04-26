#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

typedef struct Node
{
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

struct Maze
{
    int rows, cols;
    struct Cell **grid;

    struct Cell *start;
    struct Cell *end;

    int start_r, start_c;
    int end_r, end_c;

    Node **nodes;
};
typedef struct Maze Maze;
struct Cell
{
    bool up, down, left, right;
    int visited;
    int row, col;

    
};
typedef struct Cell Cell;

Node *buildGraph(Maze *m);

Maze* generateMazeRandomPositions(int rows, int cols);
Maze* generateImperfectMazeRandomPositions(int rows, int cols);
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