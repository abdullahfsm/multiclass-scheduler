#include "seqmanager.h"

void init_seq_manager(struct seq_manager *sm, char *file_name)
{
    FILE *fd = NULL;
    char key[80] = {0};
    char line[256] = {0};
    unsigned int i;
       
    sm->num_classes=0;

    fd = fopen(file_name, "r");

    while (fgets(line, sizeof(line), fd) != NULL)
    {
        remove_newline(line);
        sscanf(line, "%s", key);

        if (!strcmp(key, "sequencer"))
        {
            sscanf(line, "%s %s %u", key, sm->seq_addr, &(sm->seq_port));
        }
        else if (!strcmp(key, "threshold"))
        {
            sm->num_classes+=1;
        }
    }

    fclose(fd);
    fd=NULL;

    printf("DEBUG: num_classes: %d\n", sm->num_classes);

    init_conn_list(&(sm->sequencer_conns), 0, sm->seq_addr, sm->seq_port);
    insert_conn_list(&(sm->sequencer_conns), sm->num_classes);

    init_UDP_socket(&(sm->recv_fd), -1, true, true);
    init_UDP_socket(&(sm->send_fd), -1, false, true);

    sm->broadcast_listen_queues = calloc(sm->num_classes, sizeof(struct Queue));
    sm->seq_inprog = calloc(sm->num_classes, sizeof(unsigned int));

    printf("DEBUG: Queues created\n");

    for (i = 0; i < sm->num_classes; i++){
        create_queue(&(sm->broadcast_listen_queues[i]));
        sm->seq_inprog[i] = 0;
    }

    pthread_create(&(sm->broadcast_listener), NULL, run_broadcast_listener, (void *)sm);
}

/*  get seq number from sequencer - uses TCP*/
void get_seq(struct seq_manager *sm, unsigned int fc)
{

    struct queue_data *tmp;
    struct seq_msg *smsg = malloc(sizeof(struct seq_msg));
    smsg->fc = fc;

    /* Each class has its own connection to the sequencer*/
    struct conn_node *node = get_node_by_index(&(sm->sequencer_conns), smsg->fc);
    
    /* get seq from sequencer */
    send_seq_msg(node->sockfd, smsg);

    /* Reading response from sequencer */
    recv_seq_msg(node->sockfd, smsg);


    /* no need to wait if sequencer says go */
    if(smsg->seq == smsg->active){
        sm->seq_inprog[fc] = smsg->seq;
        return;
    }

    
    while(1){
        sem_wait(&(sm->broadcast_listen_queues[fc].sem_mutex));
        tmp = dequeue(&(sm->broadcast_listen_queues[fc]));
        
        if(tmp!=NULL){
            if(smsg->seq == tmp->seq){
                sm->seq_inprog[fc] = smsg->seq;
                break;
            }
        }
    }
    return;
}

/* RESPONSE HANDLER */
void release_seq(struct seq_manager *sm, unsigned int fc){
    
    struct seq_msg *smsg = malloc(sizeof (struct seq_msg));
    smsg->fc = fc;
    smsg->seq = sm->seq_inprog[fc];
    smsg->active = sm->seq_inprog[fc]+1;
    send_UDP_seq_msg("10.1.1.255", BROADCAST, sm->send_fd, smsg);
    sm->seq_inprog[fc] = 0;

    // if(use_seq && flow.id!=0 && isHeadBlocked(&dispatch_queues[fc]))
    // if(use_seq && flow.id!=0 && isHeadBlocked(&dispatch_queues[fc])){
    // #ifdef BLOGGING
    // printf("broadcasting on socket: %d\n", send_fd);
    // printf("BROADCASTING: id: %u seq: %u\n", flow.id, smsg.seq);
    // printf("=====================================\n");
    // fprintf(log_fd, "BROADCASTING: id: %u seq: %u\n", flow.id, smsg.seq);
    // fprintf(log_fd, "=====================================\n");
    // #endif
}

/* run broad cast listner */
void *run_broadcast_listener(void *ptr){
    
    struct seq_manager *sm = (struct seq_manager *)ptr;
    struct seq_msg *smsg = malloc(sizeof (struct seq_msg));
    unsigned int fc;

    printf("DEBUG: going into recv broadcast loop\n");

    while(1)
    {        
        recv_UDP_seq_msg(sm->recv_fd, smsg);
        fc = smsg->fc;
        enqueue(&(sm->broadcast_listen_queues[fc]), (struct queue_data){.id = 0,
                .seq=smsg->active, .fc=fc});
        sem_post(&(sm->broadcast_listen_queues[fc].sem_mutex));
    }
}


/* Terminate all connections to sequencer(s) */
void del_seq_manager(struct seq_manager *sm)
{

    unsigned int i;
    struct conn_node *ptr = NULL;

    for (i=0; i < sm->num_classes; i++){
        ptr = get_node_by_index(&(sm->sequencer_conns), i);
        close(ptr->sockfd);
        ptr->connected = false;
        ptr->busy = false;
    }
    clear_conn_list(&(sm->sequencer_conns));
}