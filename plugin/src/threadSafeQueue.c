#include "threadSafeQueue.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 *  A queue implemented as a linked list with a link to the first and last nodes
 *  A mutex serves to mark a critical region and synchronize changes to the queue
 *  A semaphore serves to indicate if there are elements to take from the queue and block if there are none
 **/
struct __TSQueue {
	struct __TSQueueNode *first;
	struct __TSQueueNode *last;
	pthread_mutex_t mutex;
	Semaphore *semaphore;
};

/**
 *  A node in the queue
 **/
typedef struct __TSQueueNode {
	void *element;
	struct __TSQueueNode *next;
} TSQueueNode;

/**
 *  Create a new queue in heap
 *  Returns a pointer to the newly created queue
 **/
TSQueue *make_threadsafe_queue(Semaphore *semaphore) {
	pthread_mutex_t mutex;
	if (pthread_mutex_init(&(mutex), NULL) != 0) {
		perror("pthread_mutex_init error in make_queue");
		return 0;
	}

	TSQueue *queue = (TSQueue *) malloc(sizeof(TSQueue));
	queue->mutex = mutex;
	queue->semaphore = semaphore;
	queue->first = NULL;
	queue->last = NULL;
	return queue;
}

/**
 *  Free a queue in heap and all its nodes
 *  Does not free the elements pointed by the nodes, as they are owned by the user
 *  Fails if pointer is invalid
 **/
void free_threadsafe_queue(TSQueue *queue) {
	pthread_mutex_t mutex = queue->mutex;
	pthread_mutex_lock(&mutex);
	TSQueueNode *node = queue->first;
	TSQueueNode *next_node = node;
	while (node) {
		next_node = node->next;
		free(node);
		node = next_node;
	}
	free(queue);
	pthread_mutex_unlock(&mutex);
}

/**
 *  Put an element at the end of in the thread safe queue
 *  Only one process may modify the queue at a single point in time
 *  Allocates a new node and puts the element into it
 **/
void put_threadsafe_queue(TSQueue *queue, void *element) {
	TSQueueNode *node = (TSQueueNode *) malloc(sizeof(TSQueueNode));
	node->element = element;

	pthread_mutex_lock(&(queue->mutex));
	if (!queue->first) {
		queue->first = node;
		queue->last = node;
	} else {
		queue->last->next = node;
		queue->last = node;
	}

	queue->semaphore->signal(queue->semaphore);
	pthread_mutex_unlock(&(queue->mutex));
}

/**
 *  Take the first element from the thread safe queue
 *  Blocks the calling thread if there are none elements
 *
 *  Only one process may modify the queue at a single point in time
 *  Frees the node and returns the element stored in it
 **/
void *take_threadsafe_queue(TSQueue *queue) {
	//Block until the queue has elements
	queue->semaphore->wait(queue->semaphore);

	TSQueueNode *node = queue->first;
	void *element = node->element;

	pthread_mutex_lock(&(queue->mutex));
	if (queue->first == queue->last) {
		queue->first = NULL;
		queue->last = NULL;
	} else {
		queue->first = node->next;
	}
	pthread_mutex_unlock(&(queue->mutex));
	free(node);
	return element;
}
