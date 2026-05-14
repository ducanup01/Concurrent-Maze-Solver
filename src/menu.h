#ifndef MENU
#define MENU

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

typedef struct {
    char **options;
    char **filenames;  // Full path to maze files
    int count;
    int maxOptions;
} DynamicMenu;

DynamicMenu* createDynamicMenu() {
    DynamicMenu *menu = malloc(sizeof(DynamicMenu));
    menu->maxOptions = 256;
    menu->options = malloc(menu->maxOptions * sizeof(char*));
    menu->filenames = malloc(menu->maxOptions * sizeof(char*));
    menu->count = 0;

    // Add generation options (allocate memory for these)
    char *opt1 = malloc(64);
    strcpy(opt1, "Randomize maze");
    menu->options[menu->count] = opt1;
    menu->filenames[menu->count] = NULL;
    menu->count++;

    char *opt2 = malloc(64);
    strcpy(opt2, "Randomize imperfect maze");
    menu->options[menu->count] = opt2;
    menu->filenames[menu->count] = NULL;
    menu->count++;

    // Scan saved_mazes directory
    DIR *dir = opendir("src/saved_mazes");
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL && menu->count < menu->maxOptions - 2) {
            if (strstr(entry->d_name, ".bin") != NULL) {
                char *displayName = malloc(256);
                char *filepath = malloc(512);
                
                // Extract name without .bin extension for display
                strncpy(displayName, entry->d_name, strlen(entry->d_name) - 4);
                displayName[strlen(entry->d_name) - 4] = '\0';
                
                snprintf(filepath, 512, "src/saved_mazes/%s", entry->d_name);
                
                menu->options[menu->count] = displayName;
                menu->filenames[menu->count] = filepath;
                menu->count++;
            }
        }
        closedir(dir);
    }

    // Add exit option (allocate memory)
    char *exitOpt = malloc(64);
    strcpy(exitOpt, "Exit");
    menu->options[menu->count] = exitOpt;
    menu->filenames[menu->count] = NULL;
    menu->count++;

    return menu;
}

void freeDynamicMenu(DynamicMenu *menu) {
    if (!menu) return;
    for (int i = 0; i < menu->count; i++) {
        if (menu->options[i]) free(menu->options[i]);
        if (menu->filenames[i]) free(menu->filenames[i]);
    }
    free(menu->options);
    free(menu->filenames);
    free(menu);
}

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

void printDynamicMenu(DynamicMenu *menu, int selected)
{
    printf("\033[H\033[J"); // clear screen

    printf("Use ↑ ↓ to navigate, ENTER to select\n\n");

    for (int i = 0; i < menu->count; i++)
    {
        if (i == selected)
            printf(" > \033[48;5;39m%s\033[0m\n", menu->options[i]); // highlighted
        else
            printf("   %s\n", menu->options[i]);
    }
}

typedef struct {
    int choice;
    char *mazeFile;
    int totalOptions;
} MenuResult;

MenuResult runDynamicMenu() {
    DynamicMenu *menu = createDynamicMenu();
    int selected = 0;
    char c;
    MenuResult result;

    while (1)
    {
        printDynamicMenu(menu, selected);
        c = getch();

        if (c == '\033') // ESC sequence
        {
            getchar(); // skip '['
            char dir = getchar();

            if (dir == 'A') // up
                selected = (selected - 1 + menu->count) % menu->count;
            else if (dir == 'B') // down
                selected = (selected + 1) % menu->count;
        }
        else if (c == '\n') // ENTER
        {
            result.choice = selected;
            result.totalOptions = menu->count;
            
            // Copy the file path before freeing menu
            if (menu->filenames[selected] != NULL) {
                char *filepath = malloc(512);
                strcpy(filepath, menu->filenames[selected]);
                result.mazeFile = filepath;
            } else {
                result.mazeFile = NULL;
            }
            
            freeDynamicMenu(menu);
            return result;
        }
    }
}

#endif