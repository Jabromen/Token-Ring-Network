#include "queue.h"


/*
	The purpose of this function is to initialize the queue structure
	
	Returns queue struct pointer
	
*/
queue_t *initQueue() {

	queue_t *q = (queue_t *) malloc(sizeof(queue_t));

	if (!q)
		return NULL;

	q->size  = 0;
	q->start = NULL;
	q->end   = NULL;

	return q;
}


/*
	The purpose of this function is to create a new node
	
	Takes message pointer
	
	Returns queue node pointer
	
*/
node_t *newNode(const char *message) {

	node_t *node = (node_t *) malloc(sizeof(node_t));

	if (!node)
		return NULL;

	// node->message = (char *) malloc(sizeof(strlen(message) + 2));

	// if (!node->message) {
	// 	free(node);
	// 	return NULL;
	// }

	strcpy(node->message, message);
	node->next = NULL;

	return node;
}


/*
	The purpose of this function is check if the queue is empty
	
	Takes queue pointer 
	
	Returns int boolean
	
*/
int isEmpty(queue_t *q) {

	if (q->size > 0)
		return 0;

	return 1;
}


/*
	The purpose of this function is to add a new node to the queue containing
	the passed message
	
	Takes message pointer and queue pointer
	
	calls newNode
	
	Returns int boolean to error check
	
*/
int putQueue(const char *message, queue_t *q) {

	node_t *node = newNode(message);

	if (!node)
		return 1;

	if (isEmpty(q))
		q->start = node;

	else
		q->end->next = node;

	q->end = node;
	q->size++;

	return 0;
}

/*
	The purpose of this function is to remove a node from the queue
	
	Takes buffer pointer and queue pointer
	
	calls removeNode
	
*/

void popQueue(char *buffer, queue_t *q) {

	if (isEmpty(q)) {
		strcpy(buffer, "\0");
		return;
	}

	strcpy(buffer, q->start->message);
	
	removeNode(q);
}


/*
	The purpose of this function is to remove a node from the queue
	
	Takes queue pointer
	
	Calls freeNode

	
*/
void removeNode(queue_t *q) {

	node_t *rem_node = q->start;

	q->start = rem_node->next;

	if (--q->size < 1)
		q->end = NULL;

	freeNode(rem_node);
}


/*
	The purpose of this function is to free queue memory to avoid memory leaks
	
	Takes queue pointer
	
	calls isEmpty, removeNode
	
*/
void freeQueue(queue_t *q) {

	while (!isEmpty(q)) {
		removeNode(q);
	}

	free(q);
}


/*
	The purpose of this function is to free the memory of one node
	
	Takes node pointer
	
*/
void freeNode(node_t *node) {

	// free(node->message);
	free(node);
}