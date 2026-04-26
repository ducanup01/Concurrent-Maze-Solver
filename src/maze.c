#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "maze.h"
#include "stack.h"

#define IDX(r, c, cols) ((r) * (cols) + (c))

Cell createCell(int row, int col)
{
    Cell c;

    c.row = row;
    c.col = col;

    c.up = c.down = c.left = c.right = true;

    c.n_up = c.n_down = c.n_left = c.n_right = NULL;

    c.visited = false;

    c.content = ' ';

    c.isStart = false;
    c.isEnd = false;

    return c;
}

void saveMazeBinary(Maze *m, const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (!f)
    {
        perror("open");
        return;
    }

    fwrite(&m->rows, sizeof(int), 1, f);
    fwrite(&m->cols, sizeof(int), 1, f);

    // save start/end coordinates
    fwrite(&m->start_r, sizeof(int), 1, f);
    fwrite(&m->start_c, sizeof(int), 1, f);
    fwrite(&m->end_r, sizeof(int), 1, f);
    fwrite(&m->end_c, sizeof(int), 1, f);

    for (int r = 0; r < m->rows; r++)
    {
        for (int c = 0; c < m->cols; c++)
        {
            Cell *cell = &m->grid[r][c];

            unsigned char walls = 0;
            walls |= cell->up << 0;
            walls |= cell->down << 1;
            walls |= cell->left << 2;
            walls |= cell->right << 3;

            fwrite(&walls, sizeof(unsigned char), 1, f);
        }
    }

    fclose(f);
}

Maze *loadMazeBinary(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
        return NULL;

    Maze *m = malloc(sizeof(Maze));

    fread(&m->rows, sizeof(int), 1, f);
    fread(&m->cols, sizeof(int), 1, f);

    fread(&m->start_r, sizeof(int), 1, f);
    fread(&m->start_c, sizeof(int), 1, f);
    fread(&m->end_r, sizeof(int), 1, f);
    fread(&m->end_c, sizeof(int), 1, f);

    // allocate grid (NO randomness here)
    m->grid = malloc(m->rows * sizeof(Cell *));
    for (int r = 0; r < m->rows; r++)
    {
        m->grid[r] = malloc(m->cols * sizeof(Cell));
        for (int c = 0; c < m->cols; c++)
        {
            m->grid[r][c] = createCell(r, c);
            m->grid[r][c].visited = false;
        }
    }

    // load walls
    for (int r = 0; r < m->rows; r++)
    {
        for (int c = 0; c < m->cols; c++)
        {
            unsigned char walls;
            fread(&walls, sizeof(unsigned char), 1, f);

            Cell *cell = &m->grid[r][c];

            cell->up = (walls >> 0) & 1;
            cell->down = (walls >> 1) & 1;
            cell->left = (walls >> 2) & 1;
            cell->right = (walls >> 3) & 1;
        }
    }

    // THEN restore start/end
    m->start = &m->grid[m->start_r][m->start_c];
    m->end = &m->grid[m->end_r][m->end_c];

    m->start->isStart = true;
    m->end->isEnd = true;

    // ✅ NOW build graph
    buildCellConnections(m);

    fclose(f);
    return m;
}

void buildCellConnections(Maze *m)
{
    int R = m->rows;
    int C = m->cols;

    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            Cell *cell = &m->grid[r][c];

            cell->n_up = NULL;
            cell->n_down = NULL;
            cell->n_left = NULL;
            cell->n_right = NULL;

            if (!cell->up && r > 0)
                cell->n_up = &m->grid[r - 1][c];

            if (!cell->down && r < R - 1)
                cell->n_down = &m->grid[r + 1][c];

            if (!cell->left && c > 0)
                cell->n_left = &m->grid[r][c - 1];

            if (!cell->right && c < C - 1)
                cell->n_right = &m->grid[r][c + 1];
        }
    }
}

void addRandomLoops(Maze *m, float probability)
{
    for (int r = 0; r < m->rows; r++)
    {
        for (int c = 0; c < m->cols; c++)
        {
            Cell *cell = &m->grid[r][c];

            // remove RIGHT wall randomly
            if (c < m->cols - 1 && ((float)rand() / RAND_MAX) < probability)
            {
                cell->right = false;
                m->grid[r][c + 1].left = false;
            }

            // remove DOWN wall randomly
            if (r < m->rows - 1 && ((float)rand() / RAND_MAX) < probability)
            {
                cell->down = false;
                m->grid[r + 1][c].up = false;
            }
        }
    }
}

Maze* generateMazeRandomPositions(int rows, int cols)
{
    if (rows < 2 || cols < 2)
    {
        printf("Maze dimension must be at least 2x2");
        return NULL;
    }

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

    srand(time(NULL));

    // define start and end
    int randomStartingRow = rand() % rows;
    int randomStartingCol = rand() % cols;

    int randomEndingRow = rand() % rows;
    int randomEndingCol = rand() % cols;

    while (randomEndingRow == randomStartingRow || randomEndingCol == randomStartingCol)
    {
        randomEndingRow = rand() % rows;
        randomEndingCol = rand() % cols;
    }

    // these are corner positions
    // m->start = &m->grid[0][0];
    // m->end = &m->grid[rows - 1][cols - 1];

    // these are random start & end positions
    m->start = &m->grid[randomStartingRow][randomStartingCol];
    m->end = &m->grid[randomEndingRow][randomEndingCol];


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


    // randomize start and end
    randomStartingRow = rand() % rows;
    randomStartingCol = rand() % cols;

    randomEndingRow = rand() % rows;
    randomEndingCol = rand() % cols;

    while (randomEndingRow == randomStartingRow || randomEndingCol == randomStartingCol)
    {
        randomEndingRow = rand() % rows;
        randomEndingCol = rand() % cols;
    }

    m->start = &m->grid[randomStartingRow][randomStartingCol];
    m->end = &m->grid[randomEndingRow][randomEndingCol];

    m->start_r = randomStartingRow;
    m->start_c = randomStartingCol;
    m->end_r = randomEndingRow;
    m->end_c = randomEndingCol;

    buildCellConnections(m);

    freeStack(stack);

    return m;
}

Maze* generateImperfectMazeRandomPositions(int rows, int cols)
{
    if (rows < 2 || cols < 2)
    {
        printf("Maze dimension must be at least 2x2");
        return NULL;
    }

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

    srand(time(NULL));

    // define start and end
    int randomStartingRow = rand() % rows;
    int randomStartingCol = rand() % cols;

    int randomEndingRow = rand() % rows;
    int randomEndingCol = rand() % cols;

    while (randomEndingRow == randomStartingRow || randomEndingCol == randomStartingCol)
    {
        randomEndingRow = rand() % rows;
        randomEndingCol = rand() % cols;
    }

    // these are corner positions
    // m->start = &m->grid[0][0];
    // m->end = &m->grid[rows - 1][cols - 1];

    // these are random start & end positions
    m->start = &m->grid[randomStartingRow][randomStartingCol];
    m->end = &m->grid[randomEndingRow][randomEndingCol];


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

    // remove some random walls
    addRandomLoops(m, 0.15f);

    // randomize start and end
    randomStartingRow = rand() % rows;
    randomStartingCol = rand() % cols;

    randomEndingRow = rand() % rows;
    randomEndingCol = rand() % rows;

    while (randomEndingRow == randomStartingRow || randomEndingCol == randomStartingCol)
    {
        randomEndingRow = rand() % rows;
        randomEndingCol = rand() % cols;
    }

    m->start = &m->grid[randomStartingRow][randomStartingCol];
    m->end = &m->grid[randomEndingRow][randomEndingCol];

    m->start_r = randomStartingRow;
    m->start_c = randomStartingCol;
    m->end_r = randomEndingRow;
    m->end_c = randomEndingCol;

    buildCellConnections(m);

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
            else content = cell->content;

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