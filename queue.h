#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct queue_t {

	int size;
	struct node_t *start;
	struct node_t *end;

} queue_t;

typedef struct node_t {

	char message[512];
	struct node_t *next;

} node_t;

queue_t *initQueue();

node_t *newNode(const char *message);

int isEmpty(queue_t *q);

int putQueue(const char *message, queue_t *q);

void removeNode(queue_t *q);

void popQueue(char *buffer, queue_t *q);

void freeQueue(queue_t *q);

void freeNode(node_t *node);

#endif