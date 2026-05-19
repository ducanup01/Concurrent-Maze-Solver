Introduction
Our product is a terminal based concurrent maze solver that visualizes a concurrency by using a multi-treaded maze solving algorithm. You can generate or load two-dimensional mazes and deploy a defined number of POSIX threads to search for the shortest path from a start cell to an end cell using Breadth-First Search. 

The main idea behind this product was to illustrate how an operating system can utilize multiple threads working on the same data at the same time. The threads pull cells from a shared queue, process them in parallel, and apply locks to avoid conflicts and corruption. 

In the visualization each thread is given a unique color. As a thread claims a cell and checks its surroundings the map is redrawn in the terminal with that thread's color indicating its location. When the exit cell is found the program stops, traces back the shortest bath via the BFS distance values and the path is animated across the screen.



The mechanisms used:
Posis Threads - pthreads 
Mutex - lock that protects the shared BFS queue
Condition Variables - allows threads to sleep when the queue is empty 
Volatile flag - shared boolean that stops all threads once the exit cell is reached

Thread’s lifecycle:


The maze itself is a grid of Cell structs that store its wall data with pointers to navigable neighbors. Mazes are generated using a depth-first search algorithm, which produces a perfect maze with one solution. There is also an option to create an imperfect maze which randomly removes additional walls to create multiple valid paths. 

How to Install the Program:

Our program is written in C and only works on POSIX compliant systems like Linux or macOS, it requires GCC to compile the main file, pthreads library, and access to a terminal. 

Simply clone the github repo with git clone, then compile from the src directory with:

gcc src/main.c src/maze.c src/stack.c src/linkedListQueue.c -o main -lpthread

How to Use the Program:

Run the compiled binary using ./main

An interactive menu will appear in the terminal use the arrows keys to navigate and enter to select 
