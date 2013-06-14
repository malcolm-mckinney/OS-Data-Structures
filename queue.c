/*
 * Generic queue implementation.
 *
 */
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

struct node{
	struct node *next;
	struct node *prev;
	void *datum;
	int pri;
};

struct queue{
	int size;
	struct node *head;
	struct node *tail;
};

/*
 * Return an empty queue.
 */
queue_t 
queue_new() {
	queue_t q = (queue_t)malloc(sizeof(struct queue));
	if(q == NULL){
		return NULL;
	}

	q->size = 0;
	q->head = q->tail = NULL;
	return q;
}

/*
 * Prepend a void* to a queue (both specifed as parameters).  Return
 * 0 (success) or -1 (failure).
 */
int 
queue_prepend(queue_t queue, void* item) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	if((n == NULL) || (queue == NULL)){
		return -1;
	}

	n->pri = 0;
	n->datum = item;
	if(queue->size == 0){
		n->prev = n->next = NULL;
		queue->head = queue->tail = n;
	}
	else{
		n->prev = NULL;
		queue->head->prev = n;
		n->next = queue->head;
		queue->head = n;
	}
	
	queue->size++;
	return 0;
}

/*
 * Append a void* to a queue (both specifed as parameters). Return
 * 0 (success) or -1 (failure). 
 */
int 
queue_append(queue_t queue, void* item) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	if((n == NULL) || (queue == NULL)){
		return -1;
	}

	n->pri = 0;
	n->datum = item;
	if(queue->size == 0){
		n->prev = n->next = NULL;
		queue->head = queue->tail = n;
	}
	else{
		n->next = NULL;
		queue->tail->next = n;
		n->prev = queue->tail;
		queue->tail = n;
	}
	
	queue->size++;
	return 0;
}

/*
 * Dequeue and return the first void* from the queue or NULL if queue
 * is empty.  Return 0 (success) or -1 (failure).
 */
int 
queue_dequeue(queue_t queue, void** item) {
	if(queue == NULL){
		return -1;
	}
	
	if(queue->size > 0){
		*item = queue->head->datum;
		if((queue->size) > 1){
			queue->head = queue->head->next;
			free(queue->head->prev);
			queue->head->prev = NULL;
		}
		else{
			free(queue->head);
			queue->head = queue->tail = NULL;
		}
		queue->size--;
	}
	else{
		*item = NULL;
		return -1;
	}
	
	return 0;
}

/*
 * Iterate the function parameter over each element in the queue.  The
 * additional void* argument is passed to the function as its first
 * argument and the queue element is the second.  Return 0 (success)
 * or -1 (failure).
 */
int
queue_iterate(queue_t queue, PFany f, void* item) {
	struct node *curr;
	int size;
	int status = 0;
	
	if(queue == NULL){
		return -1;
	}
	
	curr = queue->head;
	for(size = queue->size; (size > 0) && (status != -1); size--){
		status = f(item, curr->datum);
		curr = curr->next;
	}
	return status;
}

/*
 * Free the queue and return 0 (success) or -1 (failure).
 */
int 
queue_free (queue_t queue) {
	if(queue == NULL){
		return 0;
	}

	while(queue->size > 0){ //frees the nodes
		queue_delete(queue, (void **)&queue->head->datum);
	}
	free(queue);
	return 0;
}

/*
 * Return the number of items in the queue.
 */
int
queue_length(queue_t queue) {
	if(queue == NULL){
		return -1;
	}
	return queue->size;
}


/* 
 * Delete the specified item from the given queue. 
 * Return -1 on error.
 */
int queue_delete(queue_t queue, void** item) {
	struct node *curr;
	int size;
	if((queue == NULL) || (item == NULL)){
		return -1;
	}

	curr = queue->head;
	for(size = queue->size; size > 0; size--){ 
		if(*item == curr->datum){
			if(queue->size == 1){
				free(queue->head);
				queue->head = queue->tail = NULL;
			}
			else if(curr == queue->head){
				curr->next->prev = NULL;
				queue->head = curr->next;
				free(curr);
			}
			else if(curr == queue->tail){
				curr->prev->next = NULL;
				queue->tail = curr->prev;
				free(curr);
			}
			else{
				curr->prev->next = curr->next;
				curr->next->prev = curr->prev;
				free(curr);
			}
			
			queue->size--;
			return 0;
		}
		else{
			curr = curr->next;
		}
	}
	return -1; //fail conditions are if queue or item is NULL or item is not in the queue
}


/*
 * Inserts, and sorts item with given priority into queue (both specifed
 * as parameters). Return 0 (success) or -1 (failure). 
 */
int 
queue_insert(queue_t queue, void* item, int pri) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	struct node *in;
	if((n == NULL) || (queue == NULL)){
		return -1;
	}

	n->pri = pri;
	n->datum = item;
	if(queue->size == 0){
		n->prev = n->next = NULL;
		queue->head = queue->tail = n;
	}
	else{
		if(n->pri <= queue->head->pri){
			n->next = queue->head;
			queue->head->prev = n;
			queue->head = n;
			n->prev = NULL;
		}
		else if(n->pri >= queue->tail->pri){
			n->prev = queue->tail;
			queue->tail->next = n;
			queue->tail = n;
			n->next = NULL;
		}
		else{
			in = queue->head;
			while(in != NULL){
				if(n->pri <= in->pri){
					n->prev = in->prev;
					n->next = in;
					in->prev->next = n;
					in->prev = n;
					break;
				}
				in = in->next;
			}
		}
	}
	queue->size++;
	return 0;
}

/*
 * Peeks at the head, without popping it.
 * Return -1 on error.
 */
int queue_peek(queue_t queue, void** item){
	if((queue == NULL) || (item == NULL)){
		return -1;
	}
	if(queue->size == 0){
		*item = NULL;
		return -1;
	}
	*item = queue->head->datum;
	return 0;
}