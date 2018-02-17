//======================================================
// threads.c
//
// COMP 3430 - Operating Systems - Assignment 2 Frogger
//
// Helper functions and wrapper for pthreads
//
// @author Andy Lun
// @date June 09, 2017
//======================================================

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "console.h"
#include "threads.h"

//=====================================
// FUNCTIONS
//=====================================

void
init_condition()
{
	int error = 0;

	error = pthread_cond_init(&condition_quit, NULL);

	if (error != 0)
	{
		exit(EXIT_FAILURE);
	}
}

void
wait_condition()
{
	pthread_cond_wait(&condition_quit, &mutex_condition);
}

void
trigger_condition()
{
	if (!end_of_game)
	{
		end_of_game = true;
		pthread_cond_signal(&condition_quit);

		finalKeypress();
	}
}

void
destroy_condition()
{
	pthread_cond_destroy(&condition_quit);
}

void
init_mutex()
{
	int error = 0;

	error += pthread_mutex_init(&mutex_condition, NULL);
	error += pthread_mutex_init(&mutex_console, NULL);
	error += pthread_mutex_init(&mutex_frog, NULL);
	error += pthread_mutex_init(&mutex_logs, NULL);

	if (error != 0)
	{
		exit(EXIT_FAILURE);
	}
}

void
lock_mutex(pthread_mutex_t* mutex)
{
	pthread_mutex_lock(mutex);
}

void
unlock_mutex(pthread_mutex_t* mutex)
{
	pthread_mutex_unlock(mutex);
}

void
destroy_mutex()
{
	pthread_mutex_destroy(&mutex_condition);
	pthread_mutex_destroy(&mutex_console);
	pthread_mutex_destroy(&mutex_frog);
	pthread_mutex_destroy(&mutex_logs);
}

pthread_t
init_thread(char* name, void* func, void* args)
{
	pthread_t thread;

	if (-1 == pthread_create(&thread, NULL, func, args))
	{
		exit(EXIT_FAILURE);
	}

	if (-1 == pthread_setname_np(thread, name))
	{
		exit(EXIT_FAILURE);
	}

	return thread;
}

void*
exit_thread()
{
	pthread_exit(NULL);
	return NULL;
}

void
join_thread(pthread_t thread)
{
	pthread_join(thread, NULL);
}

void
sleep_thread(int tick_count)
{
	int count;
	for (count = 0; count < tick_count && !end_of_game; count++)
	{
		sleepTicks(1);
	}
}
