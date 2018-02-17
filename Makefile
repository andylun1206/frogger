#!/bin/sh

########################
## CONFIGURATIONS
########################
cc = gcc
ccflags = -Wall -g -pthread -lcurses

exec = frogger

src = board.c console.c frog.c frogger.c linked_list.c log.c threads.c

########################
## COMPILATION RULES
########################

all: $(src)
	@echo "Compiling Frogger..."
	@$(cc) $(ccflags) $(src) -o $(exec)
	@echo "Build successful!"

clean:
	@echo "Removing executable..."
	@rm -rf $(exec)


