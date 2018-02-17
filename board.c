//======================================================
// board.c
//
// COMP 3430 - Operating Systems - Assignment 2 Frogger
//
// Game entity - board
//
// @author Andy Lun
// @date June 09, 2017
//======================================================

#include <pthread.h>
#include <stdlib.h>

#include "board.h"
#include "console.h"
#include "frog.h"
#include "linked_list.h"
#include "threads.h"

#define SLEEP_TICKS_REFRESH 1
#define SLEEP_TICKS_UPKEEP 1

#define NUM_SLOTS 5
#define SLOT_0_MIN 0
#define SLOT_0_MAX 6
#define SLOT_1_MIN 18
#define SLOT_1_MAX 24
#define SLOT_2_MIN 36
#define SLOT_2_MAX 42
#define SLOT_3_MIN 54
#define SLOT_3_MAX 60
#define SLOT_4_MIN 72
#define SLOT_4_MAX 78
#define SLOT_ROW 2
#define SLOT_CENTER_OFFSET 3

static bool* slots;

//=====================================
// GRAPHICS
//=====================================

char* BOARD[] = {
"                                   Lives:",
"/------\\          /------\\          /------\\          /------\\          /------\\",
"|      |          |      |          |      |          |      |          |      |",
"+      +----------+      +----------+      +----------+      +----------+      +",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"",
"",
"",
""
};

//=====================================
// FUNCTIONS
//=====================================

void
init_board()
{
	lock_mutex(&mutex_console);
		consoleInit(BOARD_ROWS, BOARD_COLS, BOARD);
	unlock_mutex(&mutex_console);

	slots = (bool*) malloc(sizeof(bool) * NUM_SLOTS);
	int num_slots;
	for (num_slots = 0; num_slots < NUM_SLOTS; num_slots++)
	{
		slots[num_slots] = false;
	}
}

void
destroy_board()
{
	free(slots);
}

bool
jump_into_slot(int frog_column)
{
	bool frog_in_slot = true;

	if (SLOT_0_MIN <= frog_column && SLOT_0_MAX >= frog_column)	//slot 0
	{
		slots[0] = true;
		draw_frog(SLOT_ROW, SLOT_0_MIN + SLOT_CENTER_OFFSET);
	}
	else if (SLOT_1_MIN <= frog_column && SLOT_1_MAX >= frog_column)	//slot 1
	{
		slots[1] = true;
		draw_frog(SLOT_ROW, SLOT_1_MIN + SLOT_CENTER_OFFSET);
	}
	else if (SLOT_2_MIN <= frog_column && SLOT_2_MAX >= frog_column)	//slot 2
	{
		slots[2] = true;
		draw_frog(SLOT_ROW, SLOT_2_MIN + SLOT_CENTER_OFFSET);
	}
	else if (SLOT_3_MIN <= frog_column && SLOT_3_MAX >= frog_column)	//slot 3
	{
		slots[3] = true;
		draw_frog(SLOT_ROW, SLOT_3_MIN + SLOT_CENTER_OFFSET);
	}
	else if (SLOT_4_MIN <= frog_column && SLOT_4_MAX >= frog_column)	//slot 4
	{
		slots[4] = true;
		draw_frog(SLOT_ROW, SLOT_4_MIN + SLOT_CENTER_OFFSET);
	}
	else	// not in a slot
	{
		frog_in_slot = false;
	}

	return frog_in_slot;
}

bool
has_empty_slot()
{
	bool no_empty_slot = true;

	int num_slot;
	for (num_slot = 0; num_slot < NUM_SLOTS; num_slot++)
	{
		no_empty_slot = no_empty_slot && slots[num_slot];
	}

	return !no_empty_slot;
}

//=====================================
// THREAD FUNCTIONS
//=====================================

void*
refresh_screen(void* args)
{
	while (!end_of_game)
	{
		lock_mutex(&mutex_console);
			consoleRefresh();
		unlock_mutex(&mutex_console);

		sleep_thread(SLEEP_TICKS_REFRESH);
	}

	return exit_thread();
}

void*
upkeep(void* frog)
{
	bool quit = false;

	while (!quit && !end_of_game)
	{
		print_frog_lives(frog);
		remove_dead_logs();

		if (!has_empty_slot())
		{
			lock_mutex(&mutex_console);
				putBanner("Winner winner, frog dinner?!");
			unlock_mutex(&mutex_console);
			quit = true;
		}
		else if (0 >= get_frog_lives(frog))
		{
			lock_mutex(&mutex_console);
				putBanner("Mission failed. We'll get em' next time!");
			unlock_mutex(&mutex_console);
			quit = true;
		}

		sleep_thread(SLEEP_TICKS_UPKEEP);
	}

	if (!quit)
	{
		lock_mutex(&mutex_console);
			putBanner("Wow. Rage quitting...");
		unlock_mutex(&mutex_console);
	}

	trigger_condition();
	disableConsole(true);

	return exit_thread();
}
