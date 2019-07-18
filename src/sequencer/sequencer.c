#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <pthread.h>
#include "../common/common.h"
#include "seq_conn.h"
#define DEBUG
#define MAXCLASS 11

struct Counter
{
    /* counter lock */
    pthread_mutex_t counter_lock;
    /* seq */
    unsigned int seq;
    /* active flow */
    unsigned int active;
};

int sequencer_port = TG_SEQ_PORT;
bool verbose_mode = false;  /* by default, we don't give more detailed output */
bool daemon_mode = false;   /* by default, we don't run the sequencer as a daemon */

/* data structures and variables for class based FIFO */
struct Counter *class_counters = NULL;
pthread_t broadcast_listener;
int recv_fd;

/* print usage of the program */
void print_usage(char *program);
/* read command line arguments */
void read_args(int argc, char *argv[]);

/* handle an incomming connection */
void* handle_connection(void* ptr);

/* overhear seq_msg */
void *run_broadcast_listener(void *ptr);

void init_sequencer();

int main(int argc, char *argv[])
{
    pid_t pid, sid;
    int listen_fd;
    struct sockaddr_in seq_addr;   /* local sequencer address */
    struct sockaddr_in cli_addr;    /* remote client address */
    int sock_opt = 1;
    pthread_t seq_thread;  /* sequencer thread */
    int* sockfd_ptr = NULL;
    socklen_t len = sizeof(struct sockaddr_in);

    /* read arguments */
    read_args(argc, argv);

    /* initializing virtual queues and vars*/
    init_sequencer();

    /* initialize local sequencer address */
    memset(&seq_addr, 0, sizeof(seq_addr));
    seq_addr.sin_family = AF_INET;
    seq_addr.sin_addr.s_addr = INADDR_ANY;    
    seq_addr.sin_port = htons(sequencer_port);

    /* initialize sequencer socket */
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
        error("Error: initialize socket");

    /* set socket options */
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(sock_opt)) < 0)
        error("Error: set SO_REUSEADDR option");
    if (setsockopt(listen_fd, IPPROTO_TCP, TCP_NODELAY, &sock_opt, sizeof(sock_opt)) < 0)
        error("ERROR: set TCP_NODELAY option");

    if (bind(listen_fd,(struct sockaddr *)&seq_addr,sizeof(struct sockaddr)) < 0)
        error("Error: bind");

    if (listen(listen_fd, TG_SERVER_BACKLOG_CONN) < 0)
        error("Error: listen");

    printf("Traffic Generator Sequencer listens on 0.0.0.0:%d\n", sequencer_port);

    /* if we run the sequencer as a daemon */
    if (daemon_mode)
    {
        /* fork off the parent process */
        pid = fork();
        /* no child process is created */
        if (pid < 0)
            exit(EXIT_FAILURE);
        /* if we get the process ID of the child process, then we can exit the parent process */
        if (pid > 0)
        {
            printf("Running Traffic Generator Sequencer as a daemon\n");
            printf("The daemon process ID: %d\n", pid);
            exit(EXIT_SUCCESS);
        }

        /* change the file mode mask */
        umask(0);

        /* create a new SID for the child process */
        sid = setsid();
        if (sid < 0)
            exit(EXIT_FAILURE);

        /* change the current working directory */
        if ((chdir("/")) < 0)
            exit(EXIT_FAILURE);

        /* close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    

    while (1)
    {
        sockfd_ptr = (int*)malloc(sizeof(int));
        if (!sockfd_ptr)
            error("Error: malloc");

        *sockfd_ptr = accept(listen_fd, (struct sockaddr *)&cli_addr, &len);
        if (*sockfd_ptr < 0)
        {
            close(listen_fd);
            free(sockfd_ptr);
            error("Error: accept");
        }
        else if (pthread_create(&seq_thread, NULL, handle_connection, (void*)sockfd_ptr) < 0)
        {
            close(listen_fd);
            free(sockfd_ptr);
            error("Error: create pthread");
        }
    }

    return 0;
}

/* handle an incomming connection */
void* handle_connection(void* ptr)
{
    int sockfd = *(int*)ptr;
    free(ptr);
    struct seq_msg *smsg = malloc(sizeof(struct seq_msg));
    unsigned int fc;

    while (1)
    {

        /* check if recvd message is a valid smsg */
        if(recv_seq_msg(sockfd, smsg)){
            fc = smsg->fc;

            pthread_mutex_lock(&(class_counters[fc].counter_lock));

            smsg->seq = class_counters[fc].seq;
            smsg->active = class_counters[fc].active;
            class_counters[fc].seq += 1;

            pthread_mutex_unlock(&(class_counters[fc].counter_lock));

            send_seq_msg(sockfd, smsg);
        }
    }

    close(sockfd);
    pthread_detach(pthread_self()); //musa: for virtual memory
    return (void*)0;
}

/* Print usage of the program */
void print_usage(char *program)
{
    printf("Usage: %s [options]\n", program);
    printf("-p <port>   port number (default %d)\n", TG_SERVER_PORT);
    printf("-v          give more detailed output (verbose)\n");
    printf("-d          run the sequencer as a daemon\n");
    printf("-h          display help information\n");
}

/* Read command line arguments */
void read_args(int argc, char *argv[])
{
    int i = 1;

    while (i < argc)
    {
        if (strlen(argv[i]) == 2 && strcmp(argv[i], "-p") == 0)
        {
            if (i+1 < argc)
            {
                sequencer_port = atoi(argv[i+1]);
                if (sequencer_port < 0 || sequencer_port > 65535)
                    error("Invalid port number");
                i += 2;
            }
            /* cannot read port number */
            else
            {
                printf("Cannot read port number\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        else if (strlen(argv[i]) == 2 && strcmp(argv[i], "-v") == 0)
        {
            verbose_mode = true;
            i += 1;
        }
        else if (strlen(argv[i]) == 2 && strcmp(argv[i], "-d") == 0)
        {
            daemon_mode = true;
            i += 1;
        }
        else if (strlen(argv[i]) == 2 && strcmp(argv[i], "-h") == 0)
        {
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
        }
        else
        {
            printf("Invalid option %s\n", argv[i]);
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}


void init_sequencer()
{

    class_counters = calloc(MAXCLASS, sizeof(struct Counter));
    unsigned int i;
    for(i=0;i<MAXCLASS;i++)
    {
        pthread_mutex_init(&(class_counters[i].counter_lock), NULL);
        class_counters[i].seq = 0;
        class_counters[i].active = 0;
    }

    init_UDP_socket(&recv_fd, -1, true, true);
    pthread_create(&(broadcast_listener), NULL, run_broadcast_listener, (void *)NULL);
}

/* run broad cast manager */
void *run_broadcast_listener(void *ptr){
    struct seq_msg *smsg = malloc(sizeof(struct seq_msg));
    unsigned int fc;
    while(1){
        recv_UDP_seq_msg(recv_fd, smsg);
        fc = smsg->fc;
        
        pthread_mutex_lock(&(class_counters[fc].counter_lock));
        if(smsg->active > class_counters[fc].active)
            class_counters[fc].active = smsg->active;
        pthread_mutex_unlock(&(class_counters[fc].counter_lock));
    }

    pthread_detach(pthread_self()); //musa: for virtual memory
    return (void*)0;
}