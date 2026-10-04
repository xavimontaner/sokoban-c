#include "sokoban.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_SIZE 256U

static bool read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return false;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    return true;
}

static bool parse_unsigned(const char *text, unsigned *value) {
    char *end = NULL;
    errno = 0;
    const unsigned long parsed = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || parsed > UINT_MAX) {
        return false;
    }
    *value = (unsigned)parsed;
    return true;
}

static bool prompt_unsigned(const char *prompt, unsigned minimum, unsigned maximum, unsigned *value) {
    char input[INPUT_SIZE];
    printf("%s", prompt);
    if (!read_line(input, sizeof(input)) || !parse_unsigned(input, value)) {
        puts("Invalid number.");
        return false;
    }
    if (*value < minimum || *value > maximum) {
        printf("Choose a number from %u to %u.\n", minimum, maximum);
        return false;
    }
    return true;
}

static void play(Game *game) {
    while (!game_is_solved(game)) {
        game_print(game);
        puts("\n1 Up | 2 Right | 3 Down | 4 Left | 5 Hint | 6 Menu");
        unsigned option = 0U;
        if (!prompt_unsigned("> ", 1U, 6U, &option)) {
            continue;
        }
        if (option == 6U) {
            return;
        }
        if (option == 5U) {
            Direction hint = DIRECTION_UP;
            if (game_best_move(game, SOKOBAN_HINT_DEPTH, &hint)) {
                printf("Suggested move: %s.\n", direction_name(hint));
            } else {
                printf("No solution found within %u moves.\n", SOKOBAN_HINT_DEPTH);
            }
            continue;
        }
        if (!game_move(game, (Direction)(option - 1U))) {
            puts("That move is blocked.");
        }
    }
    game_print(game);
    puts("\nLevel completed!");
}

static void start_new_game(Game *game) {
    unsigned level = 0U;
    while (!prompt_unsigned("Choose a level (1-4): ", 1U, SOKOBAN_LEVEL_COUNT, &level)) {
    }
    if (!game_load_level(game, level)) {
        puts("Could not load the level.");
        return;
    }
    play(game);
}

static void save_current_game(const Game *game) {
    char filename[INPUT_SIZE];
    printf("Save file: ");
    if (!read_line(filename, sizeof(filename)) || filename[0] == '\0') {
        puts("Invalid filename.");
        return;
    }
    puts(game_save(game, filename) ? "Game saved." : "Could not save the game.");
}

static void load_saved_game(Game *game) {
    char filename[INPUT_SIZE];
    printf("Save file: ");
    if (!read_line(filename, sizeof(filename)) || filename[0] == '\0') {
        puts("Invalid filename.");
        return;
    }
    if (game_load(game, filename)) {
        puts("Game loaded.");
        play(game);
    } else {
        puts("The save file is invalid or could not be opened.");
    }
}

int main(void) {
    Game game;
    game_init(&game);

    bool running = true;
    while (running) {
        puts("\nSOKOBAN");
        puts("1 New game | 2 Resume | 3 Save | 4 Load | 5 Exit");
        unsigned option = 0U;
        if (!prompt_unsigned("> ", 1U, 5U, &option)) {
            continue;
        }
        switch (option) {
            case 1U:
                start_new_game(&game);
                break;
            case 2U:
                if (game.board.cells == NULL || game_is_solved(&game)) {
                    puts("There is no unfinished game to resume.");
                } else {
                    play(&game);
                }
                break;
            case 3U:
                if (game.board.cells == NULL) {
                    puts("There is no game to save.");
                } else {
                    save_current_game(&game);
                }
                break;
            case 4U:
                load_saved_game(&game);
                break;
            case 5U:
                running = false;
                break;
            default:
                break;
        }
    }

    game_free(&game);
    return EXIT_SUCCESS;
}
