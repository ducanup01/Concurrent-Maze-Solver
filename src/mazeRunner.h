#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>
#include <unistd.h>

volatile atomic_flag mutex;

void acquire(atomic_flag* lock) {while(atomic_flag_test_and_set(lock));}

void release(atomic_flag* lock) {atomic_flag_clear(lock);}

bool mazeTraverseRecursive(Node* start)
{
    if (!start || start->visited) return false;

    start->visited = 1;

    if (start->isEndingNode) {
        printf("Exit Found\n");
        return true;
    }

    if (mazeTraverseRecursive(start->up)) return true;
    if (mazeTraverseRecursive(start->down)) return true;
    if (mazeTraverseRecursive(start->left)) return true;
    if (mazeTraverseRecursive(start->right)) return true;

    return false;
}

void mazeTraverse(Node *start)
{
    mazeTraverseRecursive(start);
}