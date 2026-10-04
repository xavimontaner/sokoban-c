#include "sokoban.h"

#include <assert.h>
#include <stdio.h>

static void test_levels_load(void) {
    Game game;
    game_init(&game);
    for (unsigned level = 1U; level <= SOKOBAN_LEVEL_COUNT; ++level) {
        assert(game_load_level(&game, level));
        assert(game.board.cells != NULL);
        assert(!game_is_solved(&game));
    }
    assert(!game_load_level(&game, 5U));
    game_free(&game);
}

static void test_move_and_solve_first_level(void) {
    Game game;
    game_init(&game);
    assert(game_load_level(&game, 1U));
    assert(game_move(&game, DIRECTION_RIGHT));
    assert(game_move(&game, DIRECTION_RIGHT));
    assert(game_move(&game, DIRECTION_RIGHT));
    assert(game_is_solved(&game));
    assert(game.moves == 3U);
    game_free(&game);
}

static void test_blocked_move_does_not_increase_score(void) {
    Game game;
    game_init(&game);
    assert(game_load_level(&game, 1U));
    assert(!game_move(&game, DIRECTION_UP));
    assert(game.moves == 0U);
    game_free(&game);
}

static void test_clone_is_independent(void) {
    Game original;
    Game clone;
    game_init(&original);
    game_init(&clone);
    assert(game_load_level(&original, 1U));
    assert(game_clone(&original, &clone));
    assert(game_move(&clone, DIRECTION_RIGHT));
    assert(original.moves == 0U);
    assert(clone.moves == 1U);
    game_free(&clone);
    game_free(&original);
}

static void test_hint_for_first_level(void) {
    Game game;
    Direction direction = DIRECTION_UP;
    game_init(&game);
    assert(game_load_level(&game, 1U));
    assert(game_best_move(&game, SOKOBAN_HINT_DEPTH, &direction));
    assert(direction == DIRECTION_RIGHT);
    game_free(&game);
}

static void test_save_and_load(void) {
    const char *filename = "test-save.sok";
    Game original;
    Game loaded;
    game_init(&original);
    game_init(&loaded);
    assert(game_load_level(&original, 2U));
    assert(game_move(&original, DIRECTION_DOWN));
    assert(game_save(&original, filename));
    assert(game_load(&loaded, filename));
    assert(loaded.level == original.level);
    assert(loaded.moves == original.moves);
    assert(loaded.board.rows == original.board.rows);
    remove(filename);
    game_free(&loaded);
    game_free(&original);
}

int main(void) {
    test_levels_load();
    test_move_and_solve_first_level();
    test_blocked_move_does_not_increase_score();
    test_clone_is_independent();
    test_hint_for_first_level();
    test_save_and_load();
    puts("All Sokoban tests passed.");
    return 0;
}
