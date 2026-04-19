#include "stack.h"
#include "maze.h"
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_CAPACITY 512

Stack* createStack(int initialCapacity)
{
    if (initialCapacity <= 0) initialCapacity = DEFAULT_CAPACITY;

    Stack *s = malloc(sizeof(Stack));
    if (!s) return NULL;

    s->cell = malloc(initialCapacity * sizeof(Cell*));
    if (!s->cell) {
        free(s); return NULL;
    }

    s->top = 0;
    s->capacity = initialCapacity;
    return s;
}

static void resize(Stack *s)
{
    s->capacity *= 2;
    s->cell = realloc(s->cell, s->capacity * sizeof(Cell*));

    if (!s->cell) {
        printf("Stack reallocation failed"); exit(1);
    }
}

void push(Stack *s, Cell* value)
{
    if (s->top == s->capacity) resize(s);

    s->cell[s->top++] = value;
}

Cell* pop(Stack *s)
{
    if (isEmpty(s)) return NULL;
    return s->cell[--s->top];
}

Cell* peek(Stack *s)
{
    if (isEmpty(s))
        return NULL;
    return s->cell[s->top - 1];
}

bool isEmpty(Stack *s)
{
    return s->top == 0;
}

void freeStack(Stack *s)
{
    if (!s) return;
    free(s->cell);
    free(s);
}