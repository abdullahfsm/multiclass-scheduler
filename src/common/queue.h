#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

struct queue_data
{
	int id;
    unsigned int size;
    unsigned int seq;
    unsigned int jc;

	// bool blocking;
	// bool batch_start;
 	// bool batch_end;
	// struct timeval start_time;
};

struct Node
{
	struct queue_data data;
	struct Node *next;
};

struct Queue
{
	struct Node* head;
	struct Node* tail;
	unsigned int length;
	unsigned long long bytes;
	sem_t sem_mutex;
    pthread_mutex_t queuelock;
    // pthread_mutex_t taillock;
};

void create_queue(struct Queue *queue);
void enqueue(struct Queue *queue, struct queue_data data);
struct queue_data *dequeue(struct Queue *queue);
// void unblockTopK(struct Queue* queue, unsigned int k, unsigned int seq);

// unsigned int bytesAhead(struct Queue* queue, unsigned int k);

// bool isHeadBlocked(struct Queue* queue);
bool isEmpty(struct Queue *queue);
// void enqueueSorted(struct Queue *queue, struct queue_data data);


int len(struct Queue *queue);

// int req_purge(struct Queue *queue, int sock, int file_id);

void print_queue(struct Queue *queue);
int destroy_queue(struct Queue *queue);
#endif
