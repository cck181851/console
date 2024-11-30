#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <time.h>

#define ROWS 15
#define COLS 20
#define BANDIT_COUNT 15
#define LIFE_PILL_COUNT 3
#define POISON_COUNT 5
#define BLOCK_COUNT 15

// Global variables
char maze[ROWS][COLS];
int warrior_x = 0, warrior_y = 0; 
int life = 3; // Warrior's initial life
int princess_x, princess_y; 
int bandits[BANDIT_COUNT][2]; // Bandit positions
int life_pills[LIFE_PILL_COUNT][2]; // Life pills positions
int poisons[POISON_COUNT][2]; // Poison positions
int blocks[BLOCK_COUNT][2]; // Random block positions
struct termios oldt, newt;

// Function to restore terminal settings
void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// Signal handler for exit
void handle_exit(int sig) {
    restore_terminal();
    printf("\nExiting...\n");
    exit(0);
}

// Function to print the maze and life
void print_maze() {
    system("clear"); // Clear the console
    printf("\033[1;34mSave the Princess Game\033[0m\n\n");
    printf("Life Points Left: %d\n", life); // Display remaining life
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c", maze[i][j]);
        }
        printf("\n");
    }
}

// Function to generate random maze 
void generate_random_maze() {
    // Initialize maze with walls and random empty spaces
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == 0 || i == ROWS - 1 || j == 0 || j == COLS - 1) {
                maze[i][j] = '#'; 
            } else {
                maze[i][j] = (rand() % 4 == 0) ? '#' : '.'; // Random walls or empty spaces
            }
        }
    }

    //Clear a path across the grid 
    for (int j = 0; j < COLS; j++) {
        maze[0][j] = '.'; 
    }
    for (int i = 0; i < ROWS; i++) {
        maze[i][COLS - 1] = '.'; 
    }

    // Place the warrior
    maze[warrior_x][warrior_y] = 'W';

    // Randomly place the princess
    do {
        princess_x = rand() % (ROWS - 2) + 1; // Random x position
        princess_y = rand() % (COLS - 2) + 1; // Random y position
    } while (maze[princess_x][princess_y] != '.'); // Ensure it's not a wall or block
     maze[princess_x][princess_y] = 'P'; // Place princess

    // Place bandits
    for (int i = 0; i < BANDIT_COUNT; i++) {
        int x, y;
        do {
            x = rand() % (ROWS - 2) + 1;
            y = rand() % (COLS - 2) + 1;
        } while (maze[x][y] != '.' || (x == princess_x && y == princess_y)); // Ensure no overlap with princess
        bandits[i][0] = x;
        bandits[i][1] = y;
        maze[x][y] = 'B';
    }

    // Place life pills 
    for (int i = 0; i < LIFE_PILL_COUNT; i++) {
        int x, y;
        do {
            x = rand() % (ROWS - 2) + 1;
            y = rand() % (COLS - 2) + 1;
        } while (maze[x][y] != '.' || (x == princess_x && y == princess_y)); // Ensure no overlap with princess
        life_pills[i][0] = x;
        life_pills[i][1] = y;
        maze[x][y] = 'L';
    }

    // Place poisons 
    for (int i = 0; i < POISON_COUNT; i++) {
        int x, y;
        do {
            x = rand() % (ROWS - 2) + 1;
            y = rand() % (COLS - 2) + 1;
        } while (maze[x][y] != '.' || (x == princess_x && y == princess_y)); // Ensure no overlap with princess
        poisons[i][0] = x;
        poisons[i][1] = y;
        maze[x][y] = 'X';
    }

    // Place blocks
    for (int i = 0; i < BLOCK_COUNT; i++) {
        int x, y;
        do {
            x = rand() % (ROWS - 2) + 1;
            y = rand() % (COLS - 2) + 1;
        } while (maze[x][y] != '.' || (x == princess_x && y == princess_y)); // Ensure no overlap with princess
        blocks[i][0] = x;
        blocks[i][1] = y;
        maze[x][y] = '#';
    }
}

// Function to move the warrior
void move_warrior(char direction) {
    int new_x = warrior_x, new_y = warrior_y;

    if (direction == 'w') new_x--;      // Move up
    else if (direction == 'a') new_y--; // Move left
    else if (direction == 's') new_x++; // Move down
    else if (direction == 'd') new_y++; // Move right

    // Check if the new position is within bounds and not a wall or block
    if (new_x >= 0 && new_x < ROWS && new_y >= 0 && new_y < COLS && maze[new_x][new_y] != '#') {
        // Check for life pill
        if (maze[new_x][new_y] == 'L') {
            life++;  // Increase life
            maze[new_x][new_y] = '.';  // Remove life pill from the maze
        }

        // Check for bandit
        if (maze[new_x][new_y] == 'B') {
            life--;   // Decrease life when encountering bandit
            maze[new_x][new_y] = '.';  // Remove bandit from the maze
        }

        // Check for poison
        if (maze[new_x][new_y] == 'X') {
            life--;  // Decrease life when stepping on poison
            maze[new_x][new_y] = '.';  // Remove poison
        }

        // Check for princess
        if (new_x == princess_x && new_y == princess_y) {
            print_maze();
            printf("Congratulations! You saved the princess!\n");            
            restore_terminal();
            exit(0);
        }

        // Update player position
        maze[warrior_x][warrior_y] = '.';
        warrior_x = new_x;
        warrior_y = new_y;
        maze[warrior_x][warrior_y] = 'W'; // Set new position

        // Check if warrior's life is zero
        if (life <= 0) {
            print_maze();
            printf("Game Over! You lost all your life points.\n");
            restore_terminal();
            exit(0);
        }
    }
}

// Function to get user input 
char get_input() {
    char ch;
    struct termios oldt, newt;

    // Save old terminal settings and apply new ones
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar(); // Get user input

    // Restore old terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

// Main function
int main() {
    // Set up terminal
    tcgetattr(STDIN_FILENO, &oldt); 
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Handle signals for exit
    signal(SIGINT, handle_exit);
    signal(SIGTERM, handle_exit);

    // Seed random number generator
    srand(time(NULL));

    // Generate the random maze
    generate_random_maze();

    // Game loop
    while (1) {
        print_maze(); // Display the maze
        char input = get_input(); // Get input

        if (input == 'q') { // Exit on 'q'
            break;
        }
        move_warrior(input); // Update warrior position
        sleep(0.33);
    }

    // Restore terminal settings and exit
    restore_terminal();
    printf("\nExiting...\n");
    return 0;
}
