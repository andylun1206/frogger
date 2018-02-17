//======================================================
// frog.h
//
// COMP 3430 - Operating Systems - Assignment 2 Frogger
//
// Header file for frog.c
//
// @author Andy Lun
// @date June 09, 2017
//======================================================

#ifndef FROG_H
#define FROG_H

#include <stdbool.h>

typedef struct frog_t frog_t;

frog_t* frog;

int get_frog_lives(const frog_t* frog);

frog_t* init_frog();
void reset_frog(frog_t* frog, bool dead_frog);
void destroy_frog(frog_t* frog);

void draw_frog(int frog_row, int frog_column);

void move_frog(frog_t* frog, int row_offset, int column_offset);
void print_frog_lives(const frog_t* frog);

void* animate_frog(void* frog);
void* jump_frog(void* frog);

#endif  // FROG_H
