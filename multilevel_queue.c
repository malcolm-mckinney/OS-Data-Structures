/*
 * Multilevel queue manipulation functions  
 */
#include "multilevel_queue.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

struct multilevel_queue{
	queue_t *levels;
	int num_lvls;
};

/*
 * Returns an empty multilevel queue with number_of_levels levels. On error should return NULL.
 */
multilevel_queue_t multilevel_queue_new(int number_of_levels)
{
    multilevel_queue_t multi;
    int i = 0;
	if (number_of_levels < 1){
        return NULL;
	}

    multi = (multilevel_queue_t)malloc(sizeof(struct multilevel_queue));
	if(multi == NULL){
        return NULL;
	}
	multi->num_lvls = number_of_levels;

	multi->levels = (queue_t *)malloc(sizeof(queue_t)*number_of_levels);
	if(multi->levels == NULL){
		return NULL;
	}

	for(; i < number_of_levels; i++){
		multi->levels[i] = queue_new();
		if(multi->levels[i] == NULL) {
			return NULL;
		}
	}
    
	return multi;
}

/*
 * Appends an void* to the multilevel queue at the specified level. Return 0 (success) or -1 (failure).
 */
int multilevel_queue_enqueue(multilevel_queue_t queue, int level, void* item)
{
	if((queue == NULL) || (level > queue->num_lvls) || (level < 0)){
		return -1;
	}
	return queue_append(queue->levels[level], item);
}

/*
 * Dequeue and return the first void* from the multilevel queue starting at the specified level. 
 * Levels wrap around so as long as there is something in the multilevel queue an item should be returned.
 * Return the level that the item was located on and that item if the multilevel queue is nonempty,
 * or -1 (failure) and NULL if queue is empty.
 */
int multilevel_queue_dequeue(multilevel_queue_t queue, int level, void** item)
{
	int i = level;
	if((queue == NULL) || (level > queue->num_lvls) || (level < 0)){
		return -1;
	}
	//tries to dequeue from current level
	queue_dequeue(queue->levels[level], item);
	i++;

	//if level was empty, continue looking in other levels
	for(; (*item == NULL) && ((i - level) < queue->num_lvls); i++) {
		queue_dequeue(queue->levels[i % queue->num_lvls], item);
	}
	
    if(*item == NULL) {
		return -1;
	}
	
    return 0;
}

/* 
 * Free the queue and return 0 (success) or -1 (failure). Do not free the queue nodes; this is
 * the responsibility of the programmer.
 */
int multilevel_queue_free(multilevel_queue_t queue)
{
	int status = 0;
	queue->num_lvls--;
	for(; queue->num_lvls >= 0; queue->num_lvls--){
		if(status == 0){
			status = queue_free(queue->levels[queue->num_lvls]);
		}
		if(status == -1){
			return -1;
		}
	}

	free(queue->levels);
	free(queue);
	return 0;
}

/*
 * Returns the length of queue in the current level
 *
 */
 int multilevel_queue_length(multilevel_queue_t queue, int level) {
    if(queue == NULL){
		return -1;
	}
	return queue_length(queue->levels[level]);
 }
 
