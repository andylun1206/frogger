//======================================================
// logs.c
//
// COMP 3430 - Operating Systems - Assignment 2 Frogger
//
// Game entity - Log
//
// @author Andy Lun
// @date June 09, 2017
//======================================================

#include <stdlib.h>

#include "board.h"
#include "console.h"
#include "frog.h"
#include "linked_list.h"
#include "log.h"
#include "threads.h"

#define NUM_LOG_GRAPHICS 4
#define LOG_ENTITY_ROWS 4
#define LOG_ENTITY_COLS 24

#define MOVE_LEFT -1
#define MOVE_RIGHT 1

#define LOG_SWITCH_GRAPHICS_COUNT 5

#define LOG_SPAWN_OFFSET_MAX 60
#define LOG_SPAWN_OFFSET_MID 30
#define LOG_SPAWN_OFFSET_MIN 5

//=====================================
// GRAPHICS
//=====================================

static char* LOG_GRAPHICS[NUM_LOG_GRAPHICS][LOG_ENTITY_ROWS + 1] = {
				{"/======================\\",
				 "|      x               |",
				 "|    x                 |",
				 "\\======================/"},
				{"/======================\\",
				 "|        x             |",
				 "|          x           |",
				 "\\======================/"},
				{"/======================\\",
				 "|             x        |",
				 "|           x          |",
				 "\\======================/"},
				{"/======================\\",
				 "|               x      |",
				 "|                 x    |",
				 "\\======================/"},
};

//=====================================
// STRUCT
//=====================================

struct log_t
{
	int graphics_index;
	int tier;
	int direction;
	int speed;
	int row;
	int column;
	bool frog_on_log;
	bool log_dead;
};

int get_log_graphics_index(const log_t* log) { return log->graphics_index; }
void set_log_graphics_index(log_t* log, int graphics_index) { log->graphics_index = graphics_index; }

int get_log_tier(const log_t* log) { return log->tier; }
void set_log_tier(log_t* log, int tier) { log->tier = tier; }

int get_log_direction(const log_t* log) { return log->direction; }
void set_log_direction(log_t* log, int direction) { log->direction = direction; }

int get_log_speed(const log_t* log) { return log->speed; }
void set_log_speed(log_t* log, int speed) { log->speed = speed; }

int get_log_row(const log_t* log) { return log->row; }
void set_log_row(log_t* log, int row) { log->row = row; }
int get_log_column(const log_t* log) { return log->column; }
void set_log_column(log_t* log, int column) { log->column = column; }

bool is_frog_on_log(const log_t* log) { return log->frog_on_log; }
void set_frog_on_log(log_t* log, bool frog_on_log) { log->frog_on_log = frog_on_log; }

bool is_log_dead(const log_t* log) { return log->log_dead; }
void set_log_dead(log_t* log, bool log_dead) { log->log_dead = log_dead; }

//=====================================
// FUNCTIONS
//=====================================

log_t*
init_log(int tier)
{
	log_t* log = (log_t*) malloc(sizeof(log_t));

	int direction;
	int column;

	switch (tier)
	{
		case 1:
		case 3:
			direction = MOVE_RIGHT;
			column = SCR_LEFT - LOG_ENTITY_COLS;
			break;
		case 2:
		case 4:
			direction = MOVE_LEFT;
			column = SCR_LEFT + BOARD_COLS;
			break;
	}

	set_log_graphics_index(log, 0);
	set_log_tier(log, tier);
	set_log_direction(log, direction);
	set_log_speed(log, ((-2 * tier) + 10));
	set_log_row(log, ((-4 * tier) + 20));
	set_log_column(log, column);
	set_frog_on_log(log, false);
	set_log_dead(log, false);

	return log;
}

void
move_log(log_t* log)
{
	char** log_tile = LOG_GRAPHICS[get_log_graphics_index(log)];
	int direction = get_log_direction(log);
	int prev_column = get_log_column(log);
	int next_column = prev_column + direction;
	int row = get_log_row(log);

	set_log_column(log, next_column);

	lock_mutex(&mutex_console);
		consoleClearImage(row, prev_column, LOG_ENTITY_ROWS, LOG_ENTITY_COLS);
		consoleDrawImage(row, next_column, log_tile, LOG_ENTITY_ROWS);
	unlock_mutex(&mutex_console);

	if (is_frog_on_log(log))
	{
		move_frog(frog, 0, get_log_direction(log));
	}
}

bool
is_visible(const log_t* log)
{
	bool visible = true;
	int column = get_log_column(log);
	int direction = get_log_direction(log);

	if ((MOVE_LEFT == direction && SCR_LEFT - LOG_ENTITY_COLS < column) ||
			(MOVE_RIGHT == direction && SCR_LEFT + BOARD_COLS > column))
	{
		visible = false;
	}

	return visible;
}

bool
check_frog_on_log(int frog_tier, int frog_column, log_t* log)
{
	bool frog_on_log = false;

	if (get_log_tier(log) == frog_tier)
	{
		int log_column = get_log_column(log);

		if (log_column < frog_column && log_column + LOG_ENTITY_COLS > frog_column)
		{
			frog_on_log = true;
		}
	}

	set_frog_on_log(log, frog_on_log);

	return frog_on_log;
}

//=====================================
// THREAD FUNCTIONS
//=====================================

void*
animate_log(void* log)
{
	int sleep_ticks = get_log_speed(log);
	int count = 0;
	int max_count = LOG_SWITCH_GRAPHICS_COUNT * (get_log_tier(log) + 1);

	while (!end_of_game && !is_visible(log))
	{
		if (max_count == count)
		{
			set_log_graphics_index(log, ((get_log_graphics_index(log) + 1) % NUM_LOG_GRAPHICS));
			count = 0;
		}
		count++;

		move_log(log);
		sleep_thread(sleep_ticks);
	}

	set_log_dead(log, true);
	if (is_frog_on_log(log))
	{
		reset_frog(frog, true);
	}

	return exit_thread();
}

void*
log_spawner(void* args)
{
	int tier = *((int*) args);
	int min_wait_time = ((-2 * tier) + 10);
	int gap_size[3] = {LOG_SPAWN_OFFSET_MIN, LOG_SPAWN_OFFSET_MID, LOG_SPAWN_OFFSET_MAX};

	while (!end_of_game)
	{
		add_log(init_log(tier));
		sleep_thread((min_wait_time * (LOG_ENTITY_COLS + gap_size[(rand() % 2)])));
	}

	return exit_thread();
}
