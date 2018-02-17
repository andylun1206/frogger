//======================================================
// threads.h
//
// COMP 3430 - Operating Systems - Assignment 2 Frogger
//
// Header file for threads.c
//
// @author Andy Lun
// @date June 09, 2017
//======================================================

#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include <stdbool.h>

#define THREAD_REFRESH_SCREEN 0
#define THREAD_FROG 1
#define THREAD_FROG_ANIMATION 2
#define THREAD_UPKEEP 3
#define NUM_THREADS 8

bool end_of_game;

pthread_cond_t condition_quit;

pthread_mutex_t mutex_condition;
pthread_mutex_t mutex_console;
pthread_mutex_t mutex_frog;
pthread_mutex_t mutex_logs;

pthread_t threads[NUM_THREADS];

void init_condition();
void wait_condition();
void trigger_condition();
void destroy_condition();

void init_mutex();
void lock_mutex(pthread_mutex_t* mutex);
void unlock_mutex(pthread_mutex_t* mutex);
void destroy_mutex();

pthread_t init_thread(char* name, void* func, void* args);
void* exit_thread();
void join_thread(pthread_t thread);
void sleep_thread(int sleep_ticks);

#endif  // THREADS_H
