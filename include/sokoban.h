#ifndef SOKOBAN_H
#define SOKOBAN_H

#include <stdbool.h>
#include <stddef.h>

#define SOKOBAN_LEVEL_COUNT 4U
#define SOKOBAN_HINT_DEPTH 10U

typedef enum {
    DIRECTION_UP = 0,
    DIRECTION_RIGHT,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_COUNT
} Direction;

typedef struct {
    size_t rows;
    size_t columns;
    char **cells;
} Board;

typedef struct {
    unsigned level;
    unsigned moves;
    Board board;
} Game;

void game_init(Game *game);
void game_free(Game *game);
bool game_load_level(Game *game, unsigned level);
bool game_clone(const Game *source, Game *destination);
bool game_move(Game *game, Direction direction);
bool game_is_solved(const Game *game);
bool game_best_move(const Game *game, unsigned max_depth, Direction *direction);
bool game_save(const Game *game, const char *filename);
bool game_load(Game *game, const char *filename);
void game_print(const Game *game);
const char *direction_name(Direction direction);

#endif
