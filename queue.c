#include "queue.h"

queue_t *initQueue() {

	queue_t *q = (queue_t *) malloc(sizeof(queue_t));

	if (!q)
		return NULL;

	q->size  = 0;
	q->start = NULL;
	q->end   = NULL;

	return q;
}

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

int isEmpty(queue_t *q) {

	if (q->size > 0)
		return 0;

	return 1;
}

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

void popQueue(char *buffer, queue_t *q) {

	if (isEmpty(q)) {
		strcpy(buffer, "\0");
		return;
	}

	strcpy(buffer, q->start->message);
	
	removeNode(q);
}

void removeNode(queue_t *q) {

	node_t *rem_node = q->start;

	q->start = rem_node->next;

	if (--q->size < 1)
		q->end = NULL;

	freeNode(rem_node);
}

void freeQueue(queue_t *q) {

	while (!isEmpty(q)) {
		removeNode(q);
	}

	free(q);
}

void freeNode(node_t *node) {

	// free(node->message);
	free(node);
}