//======================================================
// frogger.c
//
// COMP 3430 - Operating Systems - Assignment 2 Frogger
//
// A text version of the classic video game "frogger."
//
// @author Andy Lun
// @date June 04, 2017
//======================================================

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "board.h"
#include "console.h"
#include "frog.h"
#include "linked_list.h"
#include "log.h"
#include "threads.h"

int
main(int argc, char** argv)
{
	srand(time(NULL));

	init_board();
	frog = init_frog();

	init_mutex();
	init_condition();

	threads[THREAD_REFRESH_SCREEN] = init_thread("screen_refresh", refresh_screen, NULL);
	threads[THREAD_FROG] = init_thread("frog_jump", jump_frog, frog);
	threads[THREAD_FROG_ANIMATION] = init_thread("frog_animation", animate_frog, frog);
	threads[THREAD_UPKEEP] = init_thread("upkeep", upkeep, frog);


	int tier;
	for (tier = 1; tier <= MAX_LOG_TIERS; tier++)
	{
		int* args = (int*) malloc(sizeof(int));
		*args = tier;

		threads[NUM_THREADS - tier] = init_thread("log_spawner", log_spawner, args);
	}

	lock_mutex(&mutex_condition);
		while (!end_of_game)
		{
			wait_condition();
		}
	unlock_mutex(&mutex_condition);

	int thread_index;
	for (thread_index = 0; thread_index < NUM_THREADS; thread_index++)
	{
		join_thread(threads[thread_index]);
	}
	destroy_condition();
	destroy_mutex();

	destroy_frog(frog);
	destroy_board();

	consoleFinish();

	printf("done!\n");

	return EXIT_SUCCESS;
}
