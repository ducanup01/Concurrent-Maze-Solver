#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "maze.h"
#include "stack.h"

#define IDX(r, c, cols) ((r) * (cols) + (c))

// struct Cell {
//     bool up, down, left, right;
//     int visited;
//     int row, col;
// };

// struct Maze {
//     int rows, cols;
//     struct Cell **grid;

//     struct Cell *start;
//     struct Cell *end;

//     int start_r, start_c;
//     int end_r, end_c;

//     Node **nodes;
// };

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

    // restore start/end pointers from coordinates
    m->start = &m->grid[m->start_r][m->start_c];
    m->end = &m->grid[m->end_r][m->end_c];

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

    fclose(f);
    return m;
}

void createNodeGrid(Maze *m)
{
    int total = m->rows * m->cols;

    m->nodes = malloc(total * sizeof(Node *));

    for (int i = 0; i < total; i++)
    {
        m->nodes[i] = malloc(sizeof(Node));

        m->nodes[i]->up = NULL;
        m->nodes[i]->down = NULL;
        m->nodes[i]->left = NULL;
        m->nodes[i]->right = NULL;

        m->nodes[i]->visited = false;
        m->nodes[i]->isEndingNode = false;
        m->nodes[i]->isStartingNode = false;
    }
}

Node* buildGraph(Maze *m)
{
    int R = m->rows;
    int C = m->cols;

    // allocate node grid
    Node **nodes = malloc(R * C * sizeof(Node *));

    for (int i = 0; i < R * C; i++)
    {
        nodes[i] = malloc(sizeof(Node));
        nodes[i]->up = NULL;
        nodes[i]->down = NULL;
        nodes[i]->left = NULL;
        nodes[i]->right = NULL;
        nodes[i]->visited = false;
        nodes[i]->isEndingNode = false;
    }

    Node *startNode = NULL;

    // build connections
    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            Cell *cell = &m->grid[r][c];
            Node *node = nodes[r * C + c];

            node->row = r;
            node->col = c;

            // mark ending node
            if (cell == m->end)
                node->isEndingNode = true;

            // save start node
            if (cell == m->start)
                startNode = node;
                node->isStartingNode = true;

            // UP
            if (!cell->up && r > 0)
            {
                Node *n = nodes[(r - 1) * C + c];
                node->up = n;
                n->down = node;
            }

            // DOWN
            if (!cell->down && r < R - 1)
            {
                Node *n = nodes[(r + 1) * C + c];
                node->down = n;
                n->up = node;
            }

            // LEFT
            if (!cell->left && c > 0)
            {
                Node *n = nodes[r * C + (c - 1)];
                node->left = n;
                n->right = node;
            }

            // RIGHT
            if (!cell->right && c < C - 1)
            {
                Node *n = nodes[r * C + (c + 1)];
                node->right = n;
                n->left = node;
            }
        }
    }

    return startNode;
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