#ifndef STACK
#define STACK

#include "maze.h"

typedef struct {
    Cell **cell;
    int top;
    int capacity;
} Stack;

Stack* createStack(int initialCapacity);
void push(Stack *s, Cell *value);
Cell* pop(Stack *s);
Cell* peek(Stack *s);
bool isEmpty(Stack *s);

void freeStack(Stack *s);

#endif