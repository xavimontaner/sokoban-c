#include "sokoban.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOARD_SIZE 64U

static const char *const LEVEL_1[] = {
    "########",
    "#.A.B.G#",
    "########",
};

static const char *const LEVEL_2[] = {
    "########",
    "#....###",
    "#.B#.#A#",
    "#..B.GG#",
    "########",
};

static const char *const LEVEL_3[] = {
    "#######",
    "###G###",
    "###B###",
    "#GBABG#",
    "###B###",
    "###G###",
    "#######",
};

static const char *const LEVEL_4[] = {
    "#######.",
    "#.B.AG##",
    "#.XXG..#",
    "#....B.#",
    "#..#####",
    "####....",
};

static bool board_allocate(Board *board, size_t rows, size_t columns) {
    if (rows == 0U || columns == 0U || rows > MAX_BOARD_SIZE || columns > MAX_BOARD_SIZE) {
        return false;
    }

    board->cells = calloc(rows, sizeof(*board->cells));
    if (board->cells == NULL) {
        return false;
    }

    board->rows = rows;
    board->columns = columns;
    for (size_t row = 0U; row < rows; ++row) {
        board->cells[row] = calloc(columns + 1U, sizeof(*board->cells[row]));
        if (board->cells[row] == NULL) {
            for (size_t previous = 0U; previous < row; ++previous) {
                free(board->cells[previous]);
            }
            free(board->cells);
            board->cells = NULL;
            board->rows = 0U;
            board->columns = 0U;
            return false;
        }
    }
    return true;
}

static void board_free(Board *board) {
    if (board == NULL) {
        return;
    }
    for (size_t row = 0U; row < board->rows; ++row) {
        free(board->cells[row]);
    }
    free(board->cells);
    board->cells = NULL;
    board->rows = 0U;
    board->columns = 0U;
}

static bool board_from_rows(Board *board, const char *const rows[], size_t row_count) {
    const size_t columns = strlen(rows[0]);
    if (!board_allocate(board, row_count, columns)) {
        return false;
    }

    for (size_t row = 0U; row < row_count; ++row) {
        if (strlen(rows[row]) != columns) {
            board_free(board);
            return false;
        }
        memcpy(board->cells[row], rows[row], columns + 1U);
    }
    return true;
}

void game_init(Game *game) {
    if (game != NULL) {
        *game = (Game){0};
    }
}

void game_free(Game *game) {
    if (game != NULL) {
        board_free(&game->board);
        game->level = 0U;
        game->moves = 0U;
    }
}

bool game_load_level(Game *game, unsigned level) {
    if (game == NULL) {
        return false;
    }

    Game replacement = {0};
    replacement.level = level;
    bool loaded = false;

    switch (level) {
        case 1U:
            loaded = board_from_rows(&replacement.board, LEVEL_1, 3U);
            break;
        case 2U:
            loaded = board_from_rows(&replacement.board, LEVEL_2, 5U);
            break;
        case 3U:
            loaded = board_from_rows(&replacement.board, LEVEL_3, 7U);
            break;
        case 4U:
            loaded = board_from_rows(&replacement.board, LEVEL_4, 6U);
            break;
        default:
            return false;
    }

    if (!loaded) {
        return false;
    }
    game_free(game);
    *game = replacement;
    return true;
}

bool game_clone(const Game *source, Game *destination) {
    if (source == NULL || destination == NULL || source->board.cells == NULL) {
        return false;
    }

    Game clone = {.level = source->level, .moves = source->moves};
    if (!board_allocate(&clone.board, source->board.rows, source->board.columns)) {
        return false;
    }

    for (size_t row = 0U; row < source->board.rows; ++row) {
        memcpy(
            clone.board.cells[row],
            source->board.cells[row],
            source->board.columns + 1U
        );
    }
    game_free(destination);
    *destination = clone;
    return true;
}

static bool find_player(const Board *board, size_t *player_row, size_t *player_column) {
    for (size_t row = 0U; row < board->rows; ++row) {
        for (size_t column = 0U; column < board->columns; ++column) {
            const char cell = board->cells[row][column];
            if (cell == 'A' || cell == 'Y') {
                *player_row = row;
                *player_column = column;
                return true;
            }
        }
    }
    return false;
}

bool game_move(Game *game, Direction direction) {
    static const int row_delta[DIRECTION_COUNT] = {-1, 0, 1, 0};
    static const int column_delta[DIRECTION_COUNT] = {0, 1, 0, -1};

    if (game == NULL || game->board.cells == NULL || direction >= DIRECTION_COUNT) {
        return false;
    }

    size_t player_row = 0U;
    size_t player_column = 0U;
    if (!find_player(&game->board, &player_row, &player_column)) {
        return false;
    }

    const int next_row = (int)player_row + row_delta[direction];
    const int next_column = (int)player_column + column_delta[direction];
    if (next_row < 0 || next_column < 0 ||
        (size_t)next_row >= game->board.rows ||
        (size_t)next_column >= game->board.columns) {
        return false;
    }

    char *next = &game->board.cells[next_row][next_column];
    if (*next == '#') {
        return false;
    }

    if (*next == 'B' || *next == 'X') {
        const int beyond_row = next_row + row_delta[direction];
        const int beyond_column = next_column + column_delta[direction];
        if (beyond_row < 0 || beyond_column < 0 ||
            (size_t)beyond_row >= game->board.rows ||
            (size_t)beyond_column >= game->board.columns) {
            return false;
        }

        char *beyond = &game->board.cells[beyond_row][beyond_column];
        if (*beyond != '.' && *beyond != 'G') {
            return false;
        }
        *beyond = (*beyond == 'G') ? 'X' : 'B';
        *next = (*next == 'X') ? 'G' : '.';
    }

    char *current = &game->board.cells[player_row][player_column];
    *current = (*current == 'Y') ? 'G' : '.';
    *next = (*next == 'G') ? 'Y' : 'A';
    ++game->moves;
    return true;
}

bool game_is_solved(const Game *game) {
    if (game == NULL || game->board.cells == NULL) {
        return false;
    }
    for (size_t row = 0U; row < game->board.rows; ++row) {
        if (strchr(game->board.cells[row], 'B') != NULL) {
            return false;
        }
    }
    return true;
}

static bool search_solution(const Game *game, unsigned depth, unsigned limit) {
    if (game_is_solved(game)) {
        return true;
    }
    if (depth >= limit) {
        return false;
    }

    for (Direction direction = DIRECTION_UP; direction < DIRECTION_COUNT; ++direction) {
        Game candidate = {0};
        if (!game_clone(game, &candidate)) {
            return false;
        }
        const bool moved = game_move(&candidate, direction);
        const bool solved = moved && search_solution(&candidate, depth + 1U, limit);
        game_free(&candidate);
        if (solved) {
            return true;
        }
    }
    return false;
}

bool game_best_move(const Game *game, unsigned max_depth, Direction *direction) {
    if (game == NULL || direction == NULL || max_depth == 0U) {
        return false;
    }

    for (unsigned limit = 1U; limit <= max_depth; ++limit) {
        for (Direction candidate_direction = DIRECTION_UP;
             candidate_direction < DIRECTION_COUNT;
             ++candidate_direction) {
            Game candidate = {0};
            if (!game_clone(game, &candidate)) {
                return false;
            }
            const bool moved = game_move(&candidate, candidate_direction);
            const bool solved = moved && search_solution(&candidate, 1U, limit);
            game_free(&candidate);
            if (solved) {
                *direction = candidate_direction;
                return true;
            }
        }
    }
    return false;
}

static bool valid_cell(char cell) {
    return strchr("#.GAYBX", cell) != NULL;
}

bool game_save(const Game *game, const char *filename) {
    if (game == NULL || game->board.cells == NULL || filename == NULL) {
        return false;
    }
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return false;
    }

    bool ok = fprintf(file, "SOKOBAN 1\n%u %u\n%zu %zu\n",
                      game->level, game->moves,
                      game->board.rows, game->board.columns) > 0;
    for (size_t row = 0U; ok && row < game->board.rows; ++row) {
        ok = fprintf(file, "%s\n", game->board.cells[row]) > 0;
    }
    if (fclose(file) != 0) {
        ok = false;
    }
    return ok;
}

bool game_load(Game *game, const char *filename) {
    if (game == NULL || filename == NULL) {
        return false;
    }
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return false;
    }

    Game loaded = {0};
    unsigned version = 0U;
    bool ok = fscanf(file, "SOKOBAN %u", &version) == 1 && version == 1U;
    ok = ok && fscanf(file, "%u %u", &loaded.level, &loaded.moves) == 2;
    ok = ok && fscanf(file, "%zu %zu", &loaded.board.rows, &loaded.board.columns) == 2;
    ok = ok && loaded.level >= 1U && loaded.level <= SOKOBAN_LEVEL_COUNT;
    ok = ok && loaded.board.rows > 0U && loaded.board.rows <= MAX_BOARD_SIZE;
    ok = ok && loaded.board.columns > 0U && loaded.board.columns <= MAX_BOARD_SIZE;

    const size_t rows = loaded.board.rows;
    const size_t columns = loaded.board.columns;
    loaded.board = (Board){0};
    ok = ok && board_allocate(&loaded.board, rows, columns);

    char buffer[MAX_BOARD_SIZE + 1U];
    size_t players = 0U;
    for (size_t row = 0U; ok && row < rows; ++row) {
        ok = fscanf(file, "%64s", buffer) == 1 && strlen(buffer) == columns;
        for (size_t column = 0U; ok && column < columns; ++column) {
            ok = valid_cell(buffer[column]);
            if (buffer[column] == 'A' || buffer[column] == 'Y') {
                ++players;
            }
        }
        if (ok) {
            memcpy(loaded.board.cells[row], buffer, columns + 1U);
        }
    }
    ok = ok && players == 1U;
    fclose(file);

    if (!ok) {
        game_free(&loaded);
        return false;
    }
    game_free(game);
    *game = loaded;
    return true;
}

void game_print(const Game *game) {
    if (game == NULL || game->board.cells == NULL) {
        puts("No game is currently loaded.");
        return;
    }
    printf("\nLevel %u | Moves: %u\n", game->level, game->moves);
    for (size_t row = 0U; row < game->board.rows; ++row) {
        puts(game->board.cells[row]);
    }
}

const char *direction_name(Direction direction) {
    static const char *const names[DIRECTION_COUNT] = {
        "up", "right", "down", "left"
    };
    return direction < DIRECTION_COUNT ? names[direction] : "unknown";
}
