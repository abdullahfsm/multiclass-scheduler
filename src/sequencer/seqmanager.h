#ifndef SEQMANAGER_H
#define SEQMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include "../common/queue.h"
#include "../common/common.h"
#include "../common/conn.h"
#include "seq_conn.h"

struct seq_manager
{
    /* class variables */
    unsigned int num_classes;

    /* Required queues */
    struct Queue *broadcast_listen_queues;
    unsigned int *seq_inprog;

    /* worker threads */
    pthread_t broadcast_listener;

    /* connection related stuff */
    int send_fd, recv_fd;
    struct conn_list sequencer_conns; /* TCP connection with sequencer */
    unsigned int seq_port;   /* port of sequencer */
    char seq_addr[20]; /* IP address of sequencer */

    
};

void init_seq_manager(struct seq_manager *sm, char *file_name);
void get_seq(struct seq_manager *sm, unsigned int fc);
void release_seq(struct seq_manager *sm, unsigned int fc);
void *run_broadcast_listener(void *ptr);
void del_seq_manager(struct seq_manager *sm);
#endif