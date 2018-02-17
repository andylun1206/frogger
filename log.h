//======================================================
// log.h
//
// COMP 3430 - Operating Systems - Assignment 2 Frogger
//
// Header file for log.c
//
// @author Andy Lun
// @date June 09, 2017
//======================================================

#ifndef LOG_H
#define LOG_H

#define MAX_LOG_TIERS 4

#include <stdbool.h>

typedef struct log_t log_t;

bool is_log_dead(const log_t* log);

bool check_frog_on_log(int frog_tier, int frog_column, log_t* log);

void* animate_log(void* log);
void* log_spawner(void* args);

#endif  // LOG_H
