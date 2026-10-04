# Sokoban in C

A terminal implementation of the classic Sokoban puzzle written in C. Move the player through four built-in levels, push every box onto a goal, save and resume games, or request a suggested move from the depth-limited solver.

This project began as a first-year Computer Engineering assignment and was later refactored into a standalone project with safer input handling, clearer ownership of dynamically allocated memory and automated tests.

## Features

- Four built-in Sokoban levels
- Box, wall and goal collision rules
- Move counter
- Save and load support with validated files
- Depth-limited hint solver
- Dynamic board allocation and deep copies
- Automated tests for movement, solving, cloning, hints and persistence

## Build

You need a C11-compatible compiler such as GCC or Clang.

Using Make:

```bash
make
```

Or compile directly:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/main.c src/sokoban.c -o sokoban
```

## Run

On macOS or Linux:

```bash
./sokoban
```

On Windows:

```powershell
.\sokoban.exe
```

## Controls

During a game, use the numbered menu:

- `1`: move up
- `2`: move right
- `3`: move down
- `4`: move left
- `5`: request a hint
- `6`: return to the main menu

## Board symbols

| Symbol | Meaning |
| --- | --- |
| `#` | Wall |
| `.` | Empty floor |
| `G` | Goal |
| `A` | Player |
| `Y` | Player on a goal |
| `B` | Box |
| `X` | Box on a goal |

## Tests

```bash
make test
```

The project is compiled in CI with strict warnings enabled and the test executable is run automatically.

## Project structure

```text
sokoban_game/
|-- include/
|   `-- sokoban.h
|-- src/
|   |-- main.c
|   `-- sokoban.c
|-- tests/
|   `-- test_sokoban.c
|-- Makefile
`-- README.md
```

## What I learned

- Dynamic memory allocation and cleanup
- Structs, enums and modular C interfaces
- File input and output
- Recursive state exploration
- Defensive parsing and validation
- Unit testing pure game logic

## Origin

The original university exercise was developed incrementally across several programming labs. This public version reorganizes and hardens the implementation while preserving the core learning goals.
