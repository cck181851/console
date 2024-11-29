#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <time.h>

#define WIDTH 30
#define HEIGHT 10

// Snake structure
typedef struct Snake {
    int *x;
    int *y;
    int length;
    int capacity;
} Snake;

// Game variables
int food_x, food_y;
int score = 0;
char **board;
struct termios oldt, newt;
Snake snake;

// Function to restore terminal settings
void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// Signal handler for graceful exit
void handle_exit(int sig) {
    restore_terminal();
    printf("\nGame exited gracefully.\n");
    free(snake.x);
    free(snake.y);
    for (int i = 0; i < HEIGHT; i++) {
        free(board[i]);
    }
    free(board);
    exit(0);
}

// Function to clear the screen
void clear_screen() {
    printf("\033[H\033[J");
}

// Function to print the game board
void print_board() {
    clear_screen();
    printf("Score: %d\n", score);

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int printed = 0;

            // Print snake head
            if (snake.x[0] == j && snake.y[0] == i) {
                printf("O");
                printed = 1;
            }

            // Print snake body (tail)
            for (int k = 1; k < snake.length; k++) {
                if (snake.x[k] == j && snake.y[k] == i) {
                    printf("#");
                    printed = 1;
                    break;
                }
            }

            // Print food (bait)
            if (!printed) {
                if (food_x == j && food_y == i) {
                    printf("X");
                    printed = 1;
                }
            }

            // Print empty space
            if (!printed) {
                printf(".");
            }
        }
        printf("\n");
    }
}

// Function to generate a random position for food (bait)
void generate_food() {
    food_x = rand() % WIDTH;
    food_y = rand() % HEIGHT;

    // Ensure food is not generated on the snake's body
    for (int i = 0; i < snake.length; i++) {
        if (snake.x[i] == food_x && snake.y[i] == food_y) {
            generate_food();
            return;
        }
    }
}

// Function to move the snake
int move_snake(char direction) {
    int new_x = snake.x[0];
    int new_y = snake.y[0];

    // Determine new head position based on input
    if (direction == 'w') {
        new_y--;
    } else if (direction == 'a') {
        new_x--;
    } else if (direction == 's') {
        new_y++;
    } else if (direction == 'd') {
        new_x++;
    }

    // Check if the snake hits the border or itself
    if (new_x < 0 || new_x >= WIDTH || new_y < 0 || new_y >= HEIGHT) {
        return 0; // Snake hit the border
    }
    
    // Check if the snake runs into itself
    for (int i = 1; i < snake.length; i++) {
        if (snake.x[i] == new_x && snake.y[i] == new_y) {
            return 0; // Snake hit itself
        }
    }

    // Move the snake
    for (int i = snake.length - 1; i > 0; i--) {
        snake.x[i] = snake.x[i - 1];
        snake.y[i] = snake.y[i - 1];
    }

    // Set the new head position
    snake.x[0] = new_x;
    snake.y[0] = new_y;

    // If the snake eats food, grow the snake
    if (new_x == food_x && new_y == food_y) {
        score++;
        if (snake.length >= snake.capacity) {
            snake.capacity *= 2;
            snake.x = realloc(snake.x, snake.capacity * sizeof(int));
            snake.y = realloc(snake.y, snake.capacity * sizeof(int));
        }
        snake.x[snake.length] = snake.x[snake.length - 1];
        snake.y[snake.length] = snake.y[snake.length - 1];
        snake.length++;
        generate_food();
    }

    return 1; // Move successful
}

// Function to get user input without pressing 'Enter'
char get_input() {
    char ch;
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);  // Get current terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Set new terminal settings

    ch = getchar();  // Get user input

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore old terminal settings
    return ch;
}

// Main function
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

    // Allocate memory for the snake
    snake.capacity = 10;
    snake.x = malloc(snake.capacity * sizeof(int));
    snake.y = malloc(snake.capacity * sizeof(int));

    // Initialize snake position at the middle of the board (only head)
    snake.x[0] = WIDTH / 2;
    snake.y[0] = HEIGHT / 2;
    snake.length = 1;

    // Allocate memory for the game board
    board = malloc(HEIGHT * sizeof(char *));
    for (int i = 0; i < HEIGHT; i++) {
        board[i] = malloc(WIDTH * sizeof(char));
    }

    // Generate the first food position
    generate_food();

    // Game loop
    while (1) {
        print_board();  // Display the board

        char input = get_input();  // Get user input without requiring 'Enter'
        if (input == 'q') {  // Exit on 'q'
            break;
        }

        // Try to move the snake
        int success = move_snake(input);
        if (success == 0) {
            printf("Game Over: Snake hit the border or itself. Press any key to continue...\n");
            get_input();  // Wait for user input to continue
        }
    }

    // End game
    restore_terminal();
    printf("\nGame Over! Final Score: %d\n", score);

    // Clean up dynamic memory
    free(snake.x);
    free(snake.y);
    for (int i = 0; i < HEIGHT; i++) {
        free(board[i]);
    }
    free(board);

    return 0;
}
