#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <time.h>

#define ROWS 22
#define COLS 50
#define CANDY_COUNT 10
#define POISON_COUNT 30

// Global Variables
char maze[ROWS][COLS];
int player_x = 0, player_y = 0; // Initial player position
int candies_eaten = 0; // Counter for candies eaten
struct termios oldt, newt;

// Function to Restore Terminal Settings
void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// Signal Handler for Graceful Exit
void handle_exit(int sig) {
    restore_terminal();
    printf("\nGame exited gracefully.\n");
    exit(0);
}

// Function to Print Maze and Player Info
void print_maze() {
    system("clear"); // Clear the console
    printf("Candies Collected: %d\n", candies_eaten); // Display candies collected
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c", maze[i][j]);
        }
        printf("\n");
    }
}

// Function to Generate a Random Maze
void generate_random_maze() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == 0 || i == ROWS - 1 || j == 0 || j == COLS - 1) {
                maze[i][j] = '#'; // Border walls
            } else {
                maze[i][j] = (rand() % 4 == 0) ? '#' : '.'; // Random walls or empty spaces
            }
        }
    }

    // Ensure path from start to exit (first row and last column are .)
    for (int j = 0; j < COLS; j++) {
        maze[0][j] = '.'; // First row
    }
    for (int i = 0; i < ROWS; i++) {
        maze[i][COLS - 1] = '.'; // Last column
    }

    // Ensure player position
    maze[player_x][player_y] = 'O'; // Player at the top-left
}

// Function to Place Candies, Poisons, and Exit
void place_candies_poisons_and_exit() {
    // Place candies (C)
    for (int i = 0; i < CANDY_COUNT; i++) {
        int x = rand() % (ROWS - 2) + 1;
        int y = rand() % (COLS - 2) + 1;
        if (maze[x][y] == '.') {
            maze[x][y] = 'C'; // Place candy
        }
    }

    // Place poisons (P)
    for (int i = 0; i < POISON_COUNT; i++) {
        int x = rand() % (ROWS - 2) + 1;
        int y = rand() % (COLS - 2) + 1;
        if (maze[x][y] == '.') {
            maze[x][y] = 'P'; // Place poison
        }
    }

    // Place exit (E) randomly, ensuring it is not overwritten
    int exit_x, exit_y;
    do {
        exit_x = rand() % (ROWS - 2) + 1;
        exit_y = rand() % (COLS - 2) + 1;
    } while (maze[exit_x][exit_y] != '.'); // Ensure the spot is empty
    maze[exit_x][exit_y] = 'E'; // Place the exit
}

// Function to Move Player
void move_player(char direction) {    
    int new_x = player_x, new_y = player_y;

    if (direction == 'w') new_x--;       // Move up
    else if (direction == 'a') new_y--; // Move left
    else if (direction == 's') new_x++; // Move down
    else if (direction == 'd') new_y++; // Move right

    // Check for valid move
    if (new_x >= 0 &&
        new_x < ROWS && 
        new_y >= 0 && 
        new_y < COLS &&
        (maze[new_x][new_y] == '.' || 
         maze[new_x][new_y] == 'C' || 
         maze[new_x][new_y] == 'E' ||
         maze[new_x][new_y] == 'P') 
    ){ 
        // Check if the player reaches the exit
        if (maze[new_x][new_y] == 'E') {
            print_maze();
            printf("Congratulations! You escaped the maze!\n");   
            printf("\nGame Over! Final Score: %d\n", candies_eaten);         
            restore_terminal();
            exit(0);
        }

        // Handle candy collection
        if (maze[new_x][new_y] == 'C') {
            candies_eaten++; // Increment candy count
        }

        // Handle poison encounter
        if (maze[new_x][new_y] == 'P') {
            print_maze();
            maze[new_x][new_y] = 'P';  // Leave the poison
            maze[player_x][player_y] = '.'; // Clear the old position
            printf("You stepped on a poison! Game Over.\n");
            printf("Your score: %d\n", candies_eaten);
            restore_terminal();
            exit(0);
        }

        // Update player position and clear the previous one
        maze[player_x][player_y] = '.';
        player_x = new_x;
        player_y = new_y;
        maze[player_x][player_y] = 'O'; // Set new position
    }       
}

// Function to Get User Input Without Requiring 'Enter' Key
char get_input() {
    char ch;
    struct termios oldt, newt;

    // Save old terminal settings and apply new ones
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar(); // Get user input

    // Restore old terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

// Main Function
int main() {
    // Set up terminal for non-canonical mode
    tcgetattr(STDIN_FILENO, &oldt); // Save old settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Handle signals for graceful exit
    signal(SIGINT, handle_exit);
    signal(SIGTERM, handle_exit);

    // Seed random number generator
    srand(time(NULL));

    // Generate random maze
    generate_random_maze();

    // Place candies, poisons, and the exit
    place_candies_poisons_and_exit();

    // Game loop
    while (1) {
        print_maze(); // Display the maze with candy count
        char input = get_input(); // Read user input without requiring 'Enter'

        if (input == 'q') { // Exit on 'q'
            break;
        }

        move_player(input); // Update player position        
    }

    // Restore terminal settings and exit
    restore_terminal();
    printf("\nGame exited gracefully.\n");
    return 0;
}
