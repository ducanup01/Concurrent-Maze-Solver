#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "maze.h"
#include "stack.h"

#define IDX(r, c, cols) ((r) * (cols) + (c))

// const char *threadColors[10] = {
//     "\033[41m",  // red
//     "\033[42m",  // green
//     "\033[43m",  // yellow
//     "\033[44m",  // blue
//     "\033[45m",  // magenta
//     "\033[46m",  // cyan
//     "\033[101m", // bright red
//     "\033[102m", // bright green
//     "\033[104m", // bright blue
//     "\033[105m"  // bright magenta
// };

const char *threadColors[256];

void initThreadColors()
{
    static char buffers[256][20];

    int idx = 0;

    // 1. Bright standard colors (0–15, skip darks manually if you want)
    int bright_base[] = {
        1, 2, 3, 4, 5, 6, // skip 0 (black)
        9, 10, 11, 12, 13, 14, 15};

    for (int i = 0; i < sizeof(bright_base) / sizeof(bright_base[0]) && idx < 256; i++)
    {
        snprintf(buffers[idx], sizeof(buffers[idx]),
                 "\033[48;5;%dm", bright_base[i]);
        threadColors[idx++] = buffers[idx];
    }

    // 2. Skip grayscale entirely (232–255)
    // 3. Use only "bright-ish" cube colors (avoid low RGB combos)
    for (int r = 3; r < 6 && idx < 256; r++)
    {
        for (int g = 3; g < 6 && idx < 256; g++)
        {
            for (int b = 3; b < 6 && idx < 256; b++)
            {
                int color = 16 + (36 * r) + (6 * g) + b;

                snprintf(buffers[idx], sizeof(buffers[idx]),
                         "\033[48;5;%dm", color);
                threadColors[idx++] = buffers[idx];
            }
        }
    }

    // Fill remaining slots (if any) with safe fallback bright colors
    while (idx < 256)
    {
        int fallback = 9 + (idx % 7); // cycle bright colors
        snprintf(buffers[idx], sizeof(buffers[idx]),
                 "\033[48;5;%dm", fallback);
        threadColors[idx++] = buffers[idx];
    }
}

Cell createCell(int row, int col)
{
    Cell c;

    c.row = row;
    c.col = col;

    c.n_up = c.n_down = c.n_left = c.n_right = true;

    c.up = c.down = c.left = c.right = NULL;

    c.visited = false;
    c.visitedBy = -1;

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
            walls |= cell->n_up << 0;
            walls |= cell->n_down << 1;
            walls |= cell->n_left << 2;
            walls |= cell->n_right << 3;

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

            cell->n_up = (walls >> 0) & 1;
            cell->n_down = (walls >> 1) & 1;
            cell->n_left = (walls >> 2) & 1;
            cell->n_right = (walls >> 3) & 1;
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

            cell->up = NULL;
            cell->down = NULL;
            cell->left = NULL;
            cell->right = NULL;

            if (!cell->n_up && r > 0)
                cell->up = &m->grid[r - 1][c];

            if (!cell->n_down && r < R - 1)
                cell->down = &m->grid[r + 1][c];

            if (!cell->n_left && c > 0)
                cell->left = &m->grid[r][c - 1];

            if (!cell->n_right && c < C - 1)
                cell->right = &m->grid[r][c + 1];
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

            // remove n_right wall randomly
            if (c < m->cols - 1 && ((float)rand() / RAND_MAX) < probability)
            {
                cell->n_right = false;
                m->grid[r][c + 1].n_left = false;
            }

            // remove n_down wall randomly
            if (r < m->rows - 1 && ((float)rand() / RAND_MAX) < probability)
            {
                cell->n_down = false;
                m->grid[r + 1][c].n_up = false;
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
            m->grid[r][c].visited = false;

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

    m->start->isStart = true;
    m->end->isEnd = true;

    m->start_r = randomStartingRow;
    m->start_c = randomStartingCol;
    m->end_r = randomEndingRow;
    m->end_c = randomEndingCol;

    buildCellConnections(m);

    freeStack(stack);

    for (int r = 0; r < m->rows; r++)
    {
        for (int c = 0; c < m->cols; c++)
        {
            m->grid[r][c].visited = false;
        }
    }

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
    randomEndingCol = rand() % cols;

    while (randomEndingRow == randomStartingRow || randomEndingCol == randomStartingCol)
    {
        randomEndingRow = rand() % rows;
        randomEndingCol = rand() % cols;
    }

    m->start = &m->grid[randomStartingRow][randomStartingCol];
    m->end = &m->grid[randomEndingRow][randomEndingCol];

    m->start->isStart = true;
    m->end->isEnd = true;

    m->start_r = randomStartingRow;
    m->start_c = randomStartingCol;
    m->end_r = randomEndingRow;
    m->end_c = randomEndingCol;

    buildCellConnections(m);

    freeStack(stack);

    for (int r = 0; r < m->rows; r++)
    {
        for (int c = 0; c < m->cols; c++)
        {
            m->grid[r][c].visited = false;
        }
    }

    return m;
}

void printMaze(Maze *m)
{
    printf("\033[H");

    for (int r = 0; r < m->rows; r++)
    {
        printf("\033[2K");
        
        // print top wall
        for (int c = 0; c < m->cols; c++)
        {
            if (m->grid[r][c].n_up) printf("+---");
            else printf("+   ");
        } printf("+\n");

        printf("\033[2K");

        // print cells + n_left wall
        for (int c = 0; c < m->cols; c++)
        {
            // cell
            Cell *cell = &m->grid[r][c];
            char content = ' ';

            if (cell == m->start) content = 'S';
            else if (cell == m->end) content = 'E';
            else content = cell->content;

            // n_left wall
            if (cell->n_left) printf("|");
            else printf(" ");

            //printing logic

            if (cell == m->start)
            {
                if (cell->visitedBy >= 0)
                {
                    int id = cell->visitedBy;
                    printf(" %sS\033[0m ", threadColors[id]); // colored S
                }
                else
                {
                    printf(" S "); // default before claimed
                }
            }
            else if (cell == m->end)
            {
                if (cell->visitedBy >= 0)
                {
                    int id = cell->visitedBy;
                    printf(" %sE\033[0m ", threadColors[id]); // colored E
                }
                else
                {
                    printf(" E "); // default before found
                }
            }
            else if (cell->visited)
            {
                int id = cell->visitedBy;
                printf(" %s \033[0m ", threadColors[id]); // colored block
            }
            else
            {
                printf("   ");
            }
        }
        // n_rightmost wall
        printf("|\n");
    }

    printf("\033[2K");

    for (int c = 0; c < m->cols; c++) printf("+---");

    printf("+\n");

    fflush(stdout);
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

    if (dr == 1) {a->n_up = false; b->n_down = false;}
    else if (dr == -1) {a->n_down = false; b->n_up = false;}
    else if (dc == 1) {a->n_left = false; b->n_right = false;}
    else if (dc == -1) {a->n_right = false; b->n_left = false;}
}