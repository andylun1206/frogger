//======================================================
// linked_list.c
//
// COMP 3430 - Operating Systems - Assignment 2 Frogger
//
// Log adaptation of generic linked list data structure
//
// @author Andy Lun
// @date June 09, 2017
//======================================================

#include <stdlib.h>

#include "linked_list.h"
#include "log.h"
#include "threads.h"

//=====================================
// STRUCT
//=====================================

typedef struct node_t
{
	log_t* log;
	pthread_t thread;
	struct node_t* prev;
	struct node_t* next;
} node_t;

node_t* log_list;

//=====================================
// FUNCTIONS
//=====================================

void
add_log(log_t* log)
{
	node_t* node = (node_t*) malloc(sizeof(node_t));
	node->log = log;
	node->thread = init_thread("log", animate_log, node->log);

	lock_mutex(&mutex_logs);
		node->prev = NULL;
		node->next = log_list;
		if (NULL != log_list)
		{
			log_list->prev = node;
		}
		log_list = node;
	unlock_mutex(&mutex_logs);
}

bool
add_frog(int frog_tier, int frog_column)
{
	bool valid_jump = false;
	node_t* curr = log_list;

	lock_mutex(&mutex_logs);
		while (NULL != curr)
		{
			if (check_frog_on_log(frog_tier, frog_column, curr->log))
			{
				valid_jump = true;
			}
			curr = curr->next;
		}
	unlock_mutex(&mutex_logs);

	return valid_jump;
}



void
remove_dead_logs()
{
	lock_mutex(&mutex_logs);
		if (NULL != log_list)
		{
			node_t* curr = log_list;

			while (NULL != curr)
			{
				if (is_log_dead(curr->log))
				{
					node_t* next = curr->next;

					if (NULL != curr->prev)
					{
						(curr->prev)->next = (NULL != curr->next) ? curr->next : NULL;
					}

					if (NULL != curr->next)
					{
						(curr->next)->prev = (NULL != curr->prev) ? curr->prev : NULL;
					}

					join_thread(curr->thread);
					free(curr->log);
					free(curr);

					curr = next;
				}
				else
				{
					curr = curr->next;
				}
			}
		}
	unlock_mutex(&mutex_logs);
}
