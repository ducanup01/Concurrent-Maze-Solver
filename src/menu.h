#ifndef MENU
#define MENU

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#define OPTIONS 12

char *menu[OPTIONS] = {
    "Randomize maze",
    "Randomize imperfect maze",
    "Load file 10x15",
    "Load file 15x25",
    "Load file 18x30",
    "Load file 22x35",
    "Load file 25x45",
    "Load file 30x55",
    "Load file 35x65",
    "Load file 40x75",
    "Load file 50x85",
    "Exit"
};

char getch()
{
    struct termios oldt, newt;
    char ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

void printMenu(int selected)
{
    printf("\033[H\033[J"); // clear screen

    printf("Use ↑ ↓ to navigate, ENTER to select\n\n");

    for (int i = 0; i < OPTIONS; i++)
    {
        if (i == selected)
            printf(" > \033[48;5;39m%s\033[0m\n", menu[i]); // highlighted
        else
            printf("   %s\n", menu[i]);
    }
}

int runMenu()
{
    int selected = 0;
    char c;

    while (1)
    {
        printMenu(selected);
        c = getch();

        if (c == '\033') // ESC sequence
        {
            getchar(); // skip '['
            char dir = getchar();

            if (dir == 'A') // up
                selected = (selected - 1 + OPTIONS) % OPTIONS;
            else if (dir == 'B') // down
                selected = (selected + 1) % OPTIONS;
        }
        else if (c == '\n') // ENTER
        {
            return selected;
        }
    }
}

#endif