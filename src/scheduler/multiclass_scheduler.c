#include "multiclass_scheduler.h"

#ifndef max
    #define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
    #define min(a,b) ((a) < (b) ? (a) : (b))
#endif

void init_multi_class_queue(struct multi_class_queue* mcq, char *file_name,
                            void (*process_func_ptr)(unsigned int)){
    FILE *fd = NULL;    
    char key[80] = {0};
    char line[256] = {0};
    unsigned int num_classes=0;
    unsigned int class_id, threshold, tos, mpl, use_seq;
    unsigned int i;


    fd = fopen(file_name, "r");
    
    while (fgets(line, sizeof(line), fd) != NULL)
    {
        remove_newline(line);
        sscanf(line, "%s", key);
        if(!strcmp(key, "class"))
            num_classes++;
    }

    fclose(fd);

    mcq->num_classes = num_classes; 
    mcq->thresholds = (unsigned int*)calloc(max(num_classes, 1), sizeof(unsigned int));
    mcq->use_seq = (unsigned int*)calloc(max(num_classes, 1), sizeof(unsigned int));
    mcq->tos_map = (unsigned int*)calloc(max(num_classes, 1), sizeof(unsigned int));
    mcq->multi_programming_level = (unsigned int*)calloc(max(num_classes, 1), sizeof(unsigned int));
    mcq->class_busy_mutex = (sem_t *)calloc(num_classes, sizeof(sem_t));
    mcq->dispatch_queues = (struct Queue *)calloc(num_classes, sizeof(struct Queue));
    mcq->schedulers = (pthread_t *)calloc(num_classes, sizeof(pthread_t));
    mcq->process_func_ptr=process_func_ptr;

    // print >> fd , "class %d %d %d %d" % (t, thresholds[t], tos[t], mpl[t])

    num_classes=0;


    fd = fopen(file_name, "r");
    
    while (fgets(line, sizeof(line), fd) != NULL)
    {
        remove_newline(line);
        sscanf(line, "%s", key);



        if (!strcmp(key, "class"))
        {
            
            sscanf(line, "%s %u %u %u %u %u", key, &(class_id), &(threshold),
                                        &(tos), &(mpl), &(use_seq));
            

            mcq->thresholds[class_id] = threshold;
            mcq->tos_map[class_id] = tos;
            mcq->multi_programming_level[class_id] = mpl;
            mcq->use_seq[class_id] = use_seq;


            /*
            sscanf(line, "%s %u %u %u %u", key, &(class_id), &(mcq->thresholds[class_id]),
                                        &(mcq->tos_map[class_id]), &(mcq->multi_programming_level[class_id]));
            
            
            printf("DEBUG: %u %u %u %u \n",class_id, (mcq->thresholds[class_id]),
                                        (mcq->tos_map[class_id]), (mcq->multi_programming_level[class_id]));
            */
            
            if(use_seq)
            	mcq->use_seq_any = true;
            num_classes++;
        }
    }

    fclose(fd);
    fd=NULL;

    /* last class has an infinite threshold */
    mcq->thresholds[num_classes-1] = (unsigned int)INFINITY;

    struct input *args;

    for (i = 0; i < num_classes; i++){
        args = (struct input *)malloc(sizeof(struct input));
        args->mcq = mcq;
        args->jc = i;

        create_queue(&(mcq->dispatch_queues[i]));
        sem_init(&(mcq->class_busy_mutex[i]),0,mcq->multi_programming_level[i]);
        pthread_create(&(mcq->schedulers[i]), NULL, fifo_scheduler, (void *)(args));
    }

    create_queue(&(mcq->classifying_queue));
    pthread_create(&(mcq->classifier), NULL, classify_job,(void *)(mcq));


    /* sequencer manager */
    if(mcq->use_seq_any){
        init_seq_manager(&(mcq->sm), file_name);    
    }

    debugger(mcq);
}

void *fifo_scheduler(void *ptr)
{

    struct input *args = (struct input *)ptr;
    struct multi_class_queue *mcq = args->mcq;
    unsigned int jc = args->jc;
    struct queue_data *tmp;
    
    while(1){
        sem_wait(&(mcq->dispatch_queues[jc].sem_mutex));
        sem_wait(&(mcq->class_busy_mutex[jc]));
        
        tmp = dequeue(&mcq->dispatch_queues[jc]);

        /* insert entry point of sequencer */
        if(mcq->use_seq[jc])
            get_seq(&(mcq->sm), jc);


        if(tmp!=NULL){
            mcq->process_func_ptr(tmp->id);
        }
        else{
            printf("DEBUG scheduler: tmp shouldn't be NULL\n");
        }
    }
    return (void*)0;
}

void *classify_job(void *ptr){

    struct multi_class_queue *mcq = (struct multi_class_queue *)ptr;
    struct queue_data *tmp;
    unsigned int jc;
    while(1){
        sem_wait(&(mcq->classifying_queue.sem_mutex));

        tmp = dequeue(&mcq->classifying_queue);

        if(tmp!=NULL){            
            jc = get_class(mcq, tmp->size);
            enqueue(&(mcq->dispatch_queues[jc]), (struct queue_data){.id = tmp->id, .seq=0, .size=tmp->size, .jc=jc});
            sem_post(&(mcq->dispatch_queues[jc].sem_mutex));
        }
    }
    return (void*)0;
}

void request_handler(struct multi_class_queue *mcq, struct queue_data q_data)
{
    enqueue(&(mcq->classifying_queue), q_data);
    sem_post(&(mcq->classifying_queue.sem_mutex));
}

void response_handler(struct multi_class_queue *mcq, struct queue_data q_data)
{
    unsigned jc;
    jc = get_class(mcq, q_data.size);

    /* exit point of sequencer */
    if(mcq->use_seq[jc])
        release_seq(&(mcq->sm), jc);

    sem_post(&(mcq->class_busy_mutex[jc]));
}

unsigned int get_class(struct multi_class_queue *mcq, unsigned int size)
{
    unsigned i, jc;


    for(i=0;i<mcq->num_classes;i++){
        if(size<=mcq->thresholds[i]){
            jc=i;
            return jc;
        }
    }

    return mcq->num_classes-1;
}

unsigned int get_tos(struct multi_class_queue *mcq, unsigned int size)
{
    return mcq->tos_map[get_class(mcq, size)];
}

void debugger(struct multi_class_queue *mcq)
{
    unsigned int i;
    printf("Number of classes: %d\n", mcq->num_classes);
    for(i=0; i<mcq->num_classes; i++){
        printf("thresholds[%u]: %u tos_map[%u]: %u mpl[%u]: %u use_seq[%u]: %u\n", i, mcq->thresholds[i], i, mcq->tos_map[i], i, mcq->multi_programming_level[i], i, mcq->use_seq[i]);
    }
}

void del_multi_class_queue(struct multi_class_queue *mcq)
{
    if(mcq->use_seq_any)
        del_seq_manager(&(mcq->sm));
}

// classifier queue