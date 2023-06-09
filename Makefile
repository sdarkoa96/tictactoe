# Define variable CC to be the compiler used in this project
CC = gcc

# Define CFLAGS for the flags used along with the compiler
CFLAGS = -g -Wall

# Define TARGETS to be the targets to be run when calling 'make all'
TARGETS = clean tictactoe player

# Define PHONY targets to prevent make from confusing the phony target with the same file names
.PHONY: clean all

# If no arguments are passed to make, it will attempt the default targets
default: tictactoe player

# Targets to run under 'make all'
all: $(TARGETS)

# List of targets
tictactoe: tictactoe.c
	$(CC) $(CFLAGS) $^ -o $@

player: player.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) tictactoe player
