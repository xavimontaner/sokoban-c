CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -O2
CPPFLAGS ?= -Iinclude

TARGET = sokoban

.PHONY: all test clean

all: $(TARGET)

$(TARGET): src/main.c src/sokoban.c include/sokoban.h
	$(CC) $(CPPFLAGS) $(CFLAGS) src/main.c src/sokoban.c -o $(TARGET)

test: tests/test_sokoban
	./tests/test_sokoban

tests/test_sokoban: tests/test_sokoban.c src/sokoban.c include/sokoban.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_sokoban.c src/sokoban.c -o tests/test_sokoban

clean:
	rm -f $(TARGET) $(TARGET).exe tests/test_sokoban tests/test_sokoban.exe test-save.sok
