#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "maze.h"
#include "stack.h"

struct Cell {
    bool up, down, left, right;
    bool visited;
    int row, col;
};

struct Maze {
    int rows, cols;
    struct Cell **grid;
    struct Cell *start;
    struct Cell *end;
};

Cell createCell(int row, int col)
{
    Cell c;
    c.row = row;
    c.col = col;

    c.up = true;
    c.down = true;
    c.left = true;
    c.right = true;

    c.visited = false;
    return c;
}

Maze* generateMaze(int rows, int cols)
{
    Maze *m = malloc(sizeof(Maze));
    // define maze size
    m->rows = rows;
    m->cols = cols;

    // allocate space for all cells
    m->grid = malloc(rows * sizeof(Cell *));
    for (int r = 0; r < rows; r++)
    {
        m->grid[r] = malloc(cols * sizeof(Cell));
        for (int c = 0; c < cols; c++)
        {
            m->grid[r][c] = createCell(r, c);
        }
    }

    // define start and end
    m->start = &m->grid[0][0];
    m->end = &m->grid[rows - 1][cols - 1];

    srand(time(NULL));

    Stack *stack = createStack(rows * cols);

    Cell *start = m->start;
    start->visited = true;
    push(stack, start);

    while(!isEmpty(stack))
    {
        Cell *current = peek(stack);
        int r = current->row;
        int c = current->col;

        // collect unvisited neighbors
        Cell *neighbors[4];
        int count = 0;

        if (r > 0 && !m->grid[r - 1][c].visited)
            neighbors[count++] = &m->grid[r - 1][c];

        if (r < rows - 1 && !m->grid[r + 1][c].visited)
            neighbors[count++] = &m->grid[r + 1][c];

        if (c > 0 && !m->grid[r][c - 1].visited)
            neighbors[count++] = &m->grid[r][c - 1];

        if (c < cols - 1 && !m->grid[r][c + 1].visited)
            neighbors[count++] = &m->grid[r][c + 1];

        if (count > 0)
        {
            shuffle(neighbors, count);

            Cell *next = neighbors[0];

            removeWall(current, next);

            next->visited = true;

            push(stack, next);
        } else pop(stack);
    }

    freeStack(stack);

    return m;
}

void printMaze(Maze *m)
{
    for (int r = 0; r < m->rows; r++)
    {
        // print top wall
        for (int c = 0; c < m->cols; c++)
        {
            if (m->grid[r][c].up) printf("+---");
            else printf("+   ");
        } printf("+\n");

        // print cells + left wall
        for (int c = 0; c < m->cols; c++)
        {
            // cell
            Cell *cell = &m->grid[r][c];
            char content = ' ';

            if (cell == m->start) content = 'S';
            else if (cell == m->end) content = 'E';

            // left wall
            if (cell->left) printf("|");
            else printf(" ");

            printf(" %c ", content);
        }
        // rightmost wall
        printf("|\n");
    }

    for (int c = 0; c < m->cols; c++) printf("+---");

    printf("+\n");
}

void freeMaze(Maze *m)
{
    if (m == NULL || m->grid == NULL) return;

    for (int r = 0; r < m->rows; r++) free(m->grid[r]);

    free(m->grid);
    free(m);
}

static void shuffle(Cell **arr, int n)
{
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        Cell *tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

static void removeWall(Cell *a, Cell *b)
{
    int dr = a->row - b->row;
    int dc = a->col - b->col;

    if (dr == 1) {a->up = false; b->down = false;}
    else if (dr == -1) {a->down = false; b->up = false;}
    else if (dc == 1) {a->left = false; b->right = false;}
    else if (dc == -1) {a->right = false; b->left = false;}
}