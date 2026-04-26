#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void solveMazeConcurrently(Cell* start)
{
    printf("%p\n", start->n_right);
}