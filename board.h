//======================================================
// board.h
//
// COMP 3430 - Operating Systems - Assignment 2 Frogger
//
// Header file for board.c
//
// @author Andy Lun
// @date June 09, 2017
//======================================================

#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#define BOARD_COLS 80
#define BOARD_ROWS 24

void init_board();
void destroy_board();

bool jump_into_slot(int frog_column);
bool has_empty_slot();

void* refresh_screen(void* args);
void* upkeep(void* args);

#endif	// BOARD_H
