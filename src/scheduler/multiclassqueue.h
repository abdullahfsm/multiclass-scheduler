#ifndef MULTICLASSQUEUE_H
#define MULTICLASSQUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include "../sequencer/seqmanager.h"
#include "../common/queue.h"
#include "../common/common.h"

struct multi_class_queue
{
    /* class variables */
    unsigned int num_classes;
    unsigned int *multi_programming_level;
    unsigned int *thresholds;
    unsigned int *tos_map;

    /* Required queues */
    struct Queue *dispatch_queues;
    struct Queue classifying_queue;

    /* needed for fifo scheduling */
    sem_t *class_busy_mutex;

    /* worker threads */
    pthread_t *schedulers;
    pthread_t classifier;

    /* processing function */
    void (*process_func_ptr)(unsigned int);

    /* sequencer manager */
    unsigned int use_seq;
    struct seq_manager sm;

};

struct input
{
  struct multi_class_queue *mcq;
  unsigned int fc; 
};

void init_multi_class_queue(struct multi_class_queue* mcq, char *file_name,
                            void (*process_func_ptr)(unsigned int));
void *fifo_scheduler(void *ptr);
void *classify_flow(void *ptr);
void request_handler(struct multi_class_queue *mcq, struct queue_data q_data);
void response_handler(struct multi_class_queue *mcq, struct queue_data q_data);
unsigned int get_class(struct multi_class_queue *mcq, unsigned int size);
unsigned int get_tos(struct multi_class_queue *mcq, unsigned int size);
void debugger(struct multi_class_queue *mcq);
void del_multi_class_queue(struct multi_class_queue *mcq);

#endif