#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

typedef struct Task {
    int a, b;
} Task;

typedef struct taskQueue {
    
} taskQueue;

Task taskQueue[256];
int taskCount = 0;

void executeTask(Task* task)
{
    int result = task->a + task->b;
    printf("The sum of %d and %d is %d\n", task->a, task->b, result);
}

int main(int argc, char* argv[])
{
    Task t1 = {
        t1.a = 5,
        t1.b = 10
    };

    executeTask(&t1);

    return 0;
}