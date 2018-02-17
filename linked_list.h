//======================================================
// linked_list.h
//
// COMP 3430 - Operating Systems - Assignment 2 Frogger
//
// Header file for linked_list.c
//
// @author Andy Lun
// @date June 09, 2017
//======================================================

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>

#include "log.h"

void add_log(log_t* log);
void remove_dead_logs();

bool add_frog(int frog_tier, int frog_row);

#endif  // LINKED_LIST_H
