// #include <stdio.h>
// #include <pthread.h>
// #include <stdlib.h>
// #include <time.h>
// #include <string.h>
// #include <unistd.h>
// #include <errno.h>

// #define THREAD_NUM 6

// pthread_mutex_t mutexQueue;
// pthread_cond_t condQueue;

// typedef struct Task {
//     int a, b;
// } Task;


// Task taskQueue[256];
// int taskCount = 0;

// void executeTask(Task* task)
// {
//     usleep(100000);
//     int result = task->a + task->b;
//     printf("The sum of %d and %d is %d\n", task->a, task->b, result);
// }

// void submitTask(Task task)
// {
//     pthread_mutex_lock(&mutexQueue);
//     taskQueue[taskCount] = task;
//     taskCount++;
//     pthread_mutex_unlock(&mutexQueue);
//     pthread_cond_signal(&condQueue);

// }

// void* startThread(void* args)
// {
//     while (1)
//     {
//         Task task;

//         pthread_mutex_lock(&mutexQueue);
//         while (taskCount == 0)
//         {
//             pthread_cond_wait(&condQueue, &mutexQueue);
//         }

//         task = taskQueue[0];

//         for (int i = 0; i < taskCount; i++)
//         {
//             taskQueue[i] = taskQueue[i+1];
//         } taskCount--;
//         pthread_mutex_unlock(&mutexQueue);

//         executeTask(&task);
//     }
// }

// int main(int argc, char* argv[])
// {
//     Task t1 = {
//         t1.a = 5,
//         t1.b = 10
//     };

//     executeTask(&t1);

//     pthread_t threadArray[THREAD_NUM];

//     pthread_mutex_init(&mutexQueue, NULL);
//     pthread_cond_init(&condQueue, NULL);

//     for (int i = 0; i < THREAD_NUM; i++)
//     {
//         if (pthread_create(&threadArray[i], NULL, &startThread, NULL) != 0)
//             perror("Failed to create the thread");
//     }

//     for (int i = 0; i < 100; i++)
//     {
//         Task realTask = {
//             realTask.a = i,
//             realTask.b = 2*i
//         };
//         submitTask(realTask);
//     }

//     for (int i = 0; i < THREAD_NUM; i++)
//     {
//         if (pthread_join(threadArray[i], NULL) != 0)
//             perror("Failed to join the thread");
//     }

//     pthread_mutex_destroy(&mutexQueue);
//     pthread_cond_destroy(&condQueue);

//     return 0;
// }