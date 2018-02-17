//======================================================
// frog.c
//
// COMP 3430 - Operating Systems - Assignment 2 Frogger
//
// Game Entity - Frog
//
// @author Andy Lun
// @date June 04, 2017
//======================================================

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "board.h"
#include "console.h"
#include "frog.h"
#include "linked_list.h"
#include "log.h"
#include "threads.h"

#define NUM_FROG_GRAPHICS 2
#define FROG_ENTITY_SIZE 2

#define INIT_FROG_ROWS BOARD_ROWS - FROG_ENTITY_SIZE - 1
#define INIT_FROG_COLS BOARD_COLS/2 - FROG_ENTITY_SIZE/2

#define MAX_FROG_LIVES 4

#define SLEEP_TICKS_FROG 100

//=====================================
// GRAPHICS
//=====================================

static char* FROG_GRAPHICS[NUM_FROG_GRAPHICS][FROG_ENTITY_SIZE + 1] = {
				{"@@",
				 "<>"},
				{"--",
		 		 "<>"}
};

//=====================================
// STRUCT
//=====================================

struct frog_t
{
	int graphics_index;
	int row;
	int column;
	int tier;
	int lives;
};

int get_frog_graphics_index(const frog_t* frog) { return frog->graphics_index; }
void set_frog_graphics_index(frog_t* frog, int index) { frog->graphics_index = index; }

int get_frog_row(const frog_t* frog) { return frog->row; }
void set_frog_row(frog_t* frog, int row) { frog->row = row; }
int get_frog_column(const frog_t* frog) { return frog->column; }
void set_frog_column(frog_t* frog, int column) { frog->column = column; }

int get_frog_tier(const frog_t* frog) { return frog->tier; }
void set_frog_tier(frog_t* frog, int tier) { frog->tier = tier; }

int get_frog_lives(const frog_t* frog) { return frog->lives; }
void set_frog_lives(frog_t* frog, int lives) { frog->lives = lives; }

//=====================================
// FUNCTIONS
//=====================================

frog_t*
init_frog()
{
	frog_t* frog = (frog_t*) malloc(sizeof(frog_t));

	lock_mutex(&mutex_frog);
		set_frog_graphics_index(frog, 0);
		set_frog_row(frog, INIT_FROG_ROWS);
		set_frog_column(frog, INIT_FROG_COLS);
		set_frog_tier(frog, 0);
		set_frog_lives(frog, MAX_FROG_LIVES);
	unlock_mutex(&mutex_frog);

	return frog;
}

void
reset_frog(frog_t* frog, bool dead_frog)
{
	lock_mutex(&mutex_frog);
		set_frog_row(frog, INIT_FROG_ROWS);
		set_frog_column(frog, INIT_FROG_COLS);
		set_frog_tier(frog, 0);
		int curr_frog_lives = get_frog_lives(frog);
		set_frog_lives(frog, (dead_frog) ? (curr_frog_lives - 1) : curr_frog_lives);
	unlock_mutex(&mutex_frog);

	move_frog(frog, 0, 0);
}

void
destroy_frog(frog_t* frog)
{
	free(frog);
}

int
get_frog_jump_distance(const frog_t* frog, int direction)
{
	int distance = 0;

	switch(get_frog_tier(frog))
	{
		default:
			distance = 4;
			break;
		case MAX_LOG_TIERS:
			if (UP == direction)
			{
				distance = 3;
			}
			else if (DOWN == direction)
			{
				distance = 4;
			}
			break;
	}

	return distance * direction;
}

bool
is_frog_within_bounds(int row, int column)
{
	bool within_bounds = false;

	if (SCR_LEFT <= column && SCR_LEFT + BOARD_COLS - FROG_ENTITY_SIZE >= column && 
			SCR_TOP <= row && SCR_TOP + BOARD_ROWS - FROG_ENTITY_SIZE >= row)
	{
		within_bounds = true;
	}

  return within_bounds;
}

void
draw_frog(int row, int column)
{
	lock_mutex(&mutex_frog);
		char** frog_tile = FROG_GRAPHICS[get_frog_graphics_index(frog)];
	unlock_mutex(&mutex_frog);

	lock_mutex(&mutex_console);
		consoleDrawImage(row, column, frog_tile, FROG_ENTITY_SIZE);
	unlock_mutex(&mutex_console);
}

void
move_frog(frog_t* frog, int row_offset, int column_offset)
{
	lock_mutex(&mutex_frog);
		char** frog_tile = FROG_GRAPHICS[get_frog_graphics_index(frog)];
		int prev_row = get_frog_row(frog);
		int prev_column = get_frog_column(frog);
		int next_row = prev_row + row_offset;
		int next_column = prev_column + column_offset;
		bool redraw = false;

		if (is_frog_within_bounds(next_row, next_column))
		{
			set_frog_row(frog, next_row);
			set_frog_column(frog, next_column);

			if (prev_row != next_row)
			{
				int direction = (row_offset < 0) ? 1 : -1;
				set_frog_tier(frog, (get_frog_tier(frog) + direction));
			}

			redraw = (MAX_LOG_TIERS + 1 != get_frog_tier(frog));
		}
	unlock_mutex(&mutex_frog);

	lock_mutex(&mutex_console);
		consoleClearImage(prev_row, prev_column, FROG_ENTITY_SIZE, FROG_ENTITY_SIZE);
		if (redraw)
		{
			consoleDrawImage(next_row, next_column, frog_tile, FROG_ENTITY_SIZE);
		}
		else
		{
			consoleDrawImage(prev_row, prev_column, frog_tile, FROG_ENTITY_SIZE);
		}
	unlock_mutex(&mutex_console);
}

void
switch_frog_graphics(frog_t* frog)
{
	lock_mutex(&mutex_frog);
		set_frog_graphics_index(frog, ((get_frog_graphics_index(frog) + 1) % NUM_FROG_GRAPHICS));
		char** frog_tile = FROG_GRAPHICS[get_frog_graphics_index(frog)];
		int row = get_frog_row(frog);
		int column = get_frog_column(frog);
	unlock_mutex(&mutex_frog);

	lock_mutex(&mutex_console);
		consoleClearImage(row, column, FROG_ENTITY_SIZE, FROG_ENTITY_SIZE);
		consoleDrawImage(row, column, frog_tile, FROG_ENTITY_SIZE);
	unlock_mutex(&mutex_console);
}

void
print_frog_lives(const frog_t* frog)
{
	lock_mutex(&mutex_frog);
		int lives = get_frog_lives(frog);
	unlock_mutex(&mutex_frog);

	char char_lives[2];
	snprintf(char_lives, 2, "%d", lives);

	lock_mutex(&mutex_console);
		putString (char_lives, SCR_TOP, 42, 5);
	unlock_mutex(&mutex_console);
}

bool
get_input(int* movement_buffer)
{
	char input;

	fd_set readset;
	FD_ZERO(&readset);
	FD_SET(STDIN_FILENO, &readset);

	struct timespec timeout = getTimeout(1);

	bool quit = false;
	memset(movement_buffer, 0, sizeof(int) * 2);

	if (-1 == pselect(FD_SETSIZE, &readset, NULL, NULL, &timeout, NULL))
	{
		perror("select");
		quit = true;
	}
	else
	{
		if (-1 == read(STDIN_FILENO, &input, 1))
		{
			perror("read");
			quit = true;
		}
		else
		{
			switch(input)
			{
				case 'w':
					movement_buffer[0] = UP;
					break;
				case 'a':
					movement_buffer[1] = LEFT;
					break;
				case 's':
					movement_buffer[0] = DOWN;
					break;
				case 'd':
					movement_buffer[1] = RIGHT;
					break;
				case 'q':
					quit = true;
					break;
			}
		}
	}

	return quit;
}


//=====================================
// THREAD FUNCTIONS
//=====================================

void*
animate_frog(void* frog)
{
	while (!end_of_game)
	{
		switch_frog_graphics(frog);
		sleep_thread(SLEEP_TICKS_FROG);
	}

	pthread_exit(NULL);
}

void*
jump_frog(void* frog)
{
	int* movement_buffer;
	int row;
	int column;
	bool quit = false;

	movement_buffer = (int*) malloc(sizeof(int) * 2);

	while (!quit && !end_of_game)
	{
		if (!(quit = get_input(movement_buffer)) &&
				(0 != (row = movement_buffer[0]) || 0 != (column = movement_buffer[1])))
		{
			move_frog(frog, get_frog_jump_distance(frog, row), column);

			if (!add_frog(get_frog_tier(frog), get_frog_column(frog)) && 0 != get_frog_tier(frog))
			{
				bool dead_frog = true;

				if (MAX_LOG_TIERS + 1 == get_frog_tier(frog))
				{
					dead_frog = !jump_into_slot(get_frog_column(frog));
				}

				reset_frog(frog, dead_frog);
			}
		}
	}

	trigger_condition();
	pthread_exit(NULL);
}
