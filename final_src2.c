#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <string.h>
#include <time.h>
 
#define SIZE 9

// Grid and input system
int grid[SIZE][SIZE];

// Function to restore terminal settings
struct termios oldt, newt;
void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void generate_random_sudoku() {
    srand(time(NULL));
    for(int i = 0; i < 18; i++){
        int row = rand()%SIZE;
        int col = rand()%SIZE;
        int val = rand()%9;
        grid[row][col] = val;
    }
}

// Function to print the grid
void print_grid() {
    system("clear");
    printf("\033[1;34mSudoku Game\033[0m\n\n");

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == 0) {
                printf(". ");  // Empty cells are represented by "."
            } else {
                printf("%d ", grid[i][j]);
            }

            if ((j + 1) % 3 == 0 && j != SIZE - 1) {
                printf("| ");
            }
        }

        printf("\n");

        if ((i + 1) % 3 == 0 && i != SIZE - 1) {
            printf("------|-------|------\n");
        }
    }
    printf("\n");
}

// Function to check if the current move is valid
int is_valid_move(int row, int col, int num) {
    // Check row and column
    for (int i = 0; i < SIZE; i++) {
        if (grid[row][i] == num || grid[i][col] == num) {
            return 0; // Invalid move
        }
    }

    // Check the 3x3 subgrid
    int start_row = (row / 3) * 3;
    int start_col = (col / 3) * 3;

    for (int i = start_row; i < start_row + 3; i++) {
        for (int j = start_col; j < start_col + 3; j++) {
            if (grid[i][j] == num) {
                return 0; // Invalid move
            }
        }
    }

    return 1; // Valid move
}

// Function to take user input on the fly
int get_char() {
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

// Function to take input and update the grid
void take_input() {
    int row, col, num;
    char ch;

    printf("Enter row, column , and number to fill or 'q' to quit: ");

    // Read input until 3 characters are provided or user presses 'q'
    row = col = num = 0;
    int entry_count = 0;

    while (entry_count < 3) {
        ch = get_char();

        // Check for 'q' to quit the game
        if (ch == 'q') {
            printf("\033[1;31mExiting the game...\033[0m\n");
            restore_terminal();
            exit(0);
        }

        // If it's a number, accumulate the values
        if (ch >= '1' && ch <= '9') {
            if (entry_count == 0) {
                row = ch - '0';  // Convert char to int
            } else if (entry_count == 1) {
                col = ch - '0';  
            } else if (entry_count == 2) {
                num = ch - '0';  
            }
            entry_count++;
        }
    }

    row -= 1; // Convert to zero-based index
    col -= 1; 

    // Validate the move
    if (grid[row][col] != 0) {
        printf("\033[1;31mCell already filled! You lost!\033[0m\n");
        restore_terminal();
        exit(1); // End the game if the cell was already filled
    } else if (!is_valid_move(row, col, num)) {
        printf("\033[1;31mInvalid move! You lost!\033[0m\n");
        restore_terminal();
        exit(1); // End the game if the move is invalid
    } else {
        grid[row][col] = num;
        printf("Move accepted!\n");
    }
}

// Function to check if the game is over (i.e., the grid is complete)
int is_game_over() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == 0) {
                return 0; // Game not over, still empty cells
            }
        }
    }
    return 1; // Game over, no empty cells
}

// Main function
int main() {
    // Set up terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    generate_random_sudoku();  // Generate a random grid

    // Main game loop
    while (1) {
        print_grid();

        if (is_game_over()) {
            printf("\033[1;32mCongratulations! You solved the Sudoku!\033[0m\n");
            break;
        }

        take_input();  // Get input from the user
    }

    restore_terminal();
    return 0;
}
