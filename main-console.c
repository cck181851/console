#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#define MAX_GAMES 10
#define MAX_GAME_NAME_LEN 100

int selected_game = 0;  // Keeps track of the selected game index

// Function to get user input without waiting for Enter key
char get_input() {
    struct termios oldt, newt;
    char ch;
    
    tcgetattr(STDIN_FILENO, &oldt);  // Get current terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Apply the new settings

    ch = getchar();  // Read one character of input

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore terminal settings
    return ch;
}

// Function to display the list of games
void display_games(char *games[], int game_count) {
    printf("\033[H\033[J");  // Clear screen (ANSI escape code)
    printf("=== Video Game Console ===\n");
    printf("Use 'w' and 's' to select a game, 'Enter' to start, and 'q' to exit.\n\n");

    // Display the list of games, highlighting the selected game
    for (int i = 0; i < game_count; i++) {
        if (i == selected_game) {
            printf("-> %s\n", games[i]);  // Highlight selected game
        } else {
            printf("   %s\n", games[i]);
        }
    }
}

// Signal handler for graceful exit
void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\nExiting gracefully...\n");
        exit(0);
    }
}

// Function to load games from the 'bin' directory
int load_games(char *games[]) {
    DIR *dir;
    struct dirent *entry;
    int game_count = 0;

    dir = opendir("mount");
    if (dir == NULL) {
        perror("Failed to open directory");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "game_", 5) == 0) {  // Check if file starts with "game_"
            games[game_count] = strdup(entry->d_name);  // Store game name
            game_count++;
            if (game_count >= MAX_GAMES) {
                break;  // Limit the number of games
            }
        }
    }

    closedir(dir);
    return game_count;
}

// Function to execute the selected game
void execute_game(const char *game_name) {
    printf("\033[H\033[J");  // Clear screen before launching the game
    printf("Starting game: %s\n", game_name);

    char command[MAX_GAME_NAME_LEN + 20];

    sprintf(command, "./mount/%s", game_name);
    system(command);  // Execute the game

    // Added line to display the score
    

    printf("\nGame exited. Returning to the main menu...\n");
    printf("Press any key to continue...\n");
    get_input();  // Wait for user input before returning to the menu
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    char *games[MAX_GAMES];
    int game_count = load_games(games);

    if (game_count <= 0) {
        printf("No games found in the 'bin' directory.\n");
        return 1;
    }

    char input;
    while (1) {
        display_games(games, game_count);

        // Get user input (non-blocking)
        input = get_input();

        // Handle user navigation and game start
        if (input == 'w' && selected_game > 0) {
            selected_game--;  // Move up
        } else if (input == 's' && selected_game < game_count - 1) {
            selected_game++;  // Move down
        } else if (input == 'q') {
            break;  // Exit program
        } else if (input == '\n') {
            // Start the selected game
            execute_game(games[selected_game]);
        }
    }

    // Free dynamically allocated memory for game names
    for (int i = 0; i < game_count; i++) {
        free(games[i]);
    }

    return 0;
}

    // Free dynamically allocated memory for game names
    for (int i = 0; i < game_count; i++) {
        free(games[i]);
    }

    return 0;
}

