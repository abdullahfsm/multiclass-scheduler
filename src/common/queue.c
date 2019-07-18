#include "queue.h"

void create_queue(struct Queue *queue)
{
	queue->head = NULL;
	queue->tail = NULL;
	queue->length = 0;
	queue->bytes = 0;
    sem_init(&(queue->sem_mutex),0,0);
    pthread_mutex_init(&(queue->queuelock), NULL);
}

void enqueue(struct Queue *queue, struct queue_data data)
{
	struct Node *tmp = malloc(sizeof(struct Node));
	
	tmp->data.id = data.id;
    tmp->data.size = data.size;
    tmp->data.seq = data.seq;
    tmp->data.jc = data.jc;

    // tmp->data.blocking = data.blocking;
    // tmp->data.batch_start = data.batch_start;
    // tmp->data.batch_end = data.batch_end;
    // tmp->data.batch_end = data.batch_end;
	

	// tmp->data.start_time.tv_sec = data.start_time.tv_sec;
	// tmp->data.start_time.tv_usec = data.start_time.tv_usec;
	
	/*
	tmp->data.sock = data.sock;
	tmp->data.prio = data.prio;
	tmp->data.object_id = data.object_id;
	tmp->data.start_idx = data.start_idx;
	tmp->data.size = data.size;
	*/

	tmp->next = NULL;
	
	pthread_mutex_lock(&(queue->queuelock));
	if(queue->head == NULL) {
		queue->head = tmp;
		queue->tail = tmp;
	}
	else{
		queue->tail->next = tmp;
		queue->tail = tmp;
	}
	queue->length += 1;
	queue->bytes += data.size;
	pthread_mutex_unlock(&(queue->queuelock));

	// return queue->length;
}


// void enqueueSorted(struct Queue *queue, struct queue_data data)
// {
// 	struct Node *tmp = malloc(sizeof(struct Node));
	
// 	tmp->data.id = data.id;
//     tmp->data.blocking = data.blocking;
//     tmp->data.batch_seq = data.batch_seq;
//     tmp->data.batch_start = data.batch_start;
//     tmp->data.batch_end = data.batch_end;
//     tmp->data.size = data.size;
	
// 	tmp->next = NULL;
	
// 	pthread_mutex_lock(&(queue->queuelock));
// 	if(queue->head == NULL) {
// 		queue->head = tmp;
// 		queue->tail = tmp;
// 	}
// 	else{

// 		struct Node *curr = queue->head;
// 		struct Node *prev = NULL;

// 		while(curr!=NULL) {
			
// 			if(tmp->data.size > curr->data.size) {
				
// 				if(curr==queue->tail){
// 					curr->next = tmp;
// 					queue->tail = tmp;
// 					break;
// 				} else {
// 					prev = curr;
// 					curr = curr->next;
// 				}


// 			} else {

// 				if(curr==queue->head){
// 					tmp->next = curr;
// 					queue->head = tmp;
// 					break;
// 				} else {
// 					tmp->next = curr;
// 					prev->next = tmp;
// 					break;
// 				}
// 			}
// 		}
// 	}
// 	queue->length += 1;
// 	pthread_mutex_unlock(&(queue->queuelock));

// 	// return queue->length;
// }

struct queue_data *dequeue(struct Queue *queue)
{
	if (queue->length == 0){
		return NULL;
	}

	struct queue_data* data = malloc(sizeof(struct queue_data));;
	struct Node* tmp;


	data->id = queue->head->data.id;
    data->size = queue->head->data.size;
    data->seq = queue->head->data.seq;
    data->jc = queue->head->data.jc;



    // data->blocking = queue->head->data.blocking;
    // data->batch_start = queue->head->data.batch_start;
    // data->batch_end = queue->head->data.batch_end;
	
	tmp = queue->head;

	pthread_mutex_lock(&(queue->queuelock));
	if(queue->length == 1) {
		queue->head = NULL;
		queue->tail = NULL;
		tmp->next = NULL;
	}
	else{
		queue->head = tmp->next;
		tmp->next = NULL;		
		
	}
	queue->length -= 1;
	queue->bytes -= data->size;

	free(tmp);
	pthread_mutex_unlock(&(queue->queuelock));

	return data;
}

void print_queue(struct Queue *queue)
{
	int index = 0;
	struct Node *curr = queue->head;
	if (curr == NULL)
		printf("Info: Queue is empty\n");
	while(curr != NULL)
	{
		fprintf(stderr, "Index:%d\tID:%d\n", index, curr->data.id);
		curr = curr->next;
	}
}

// void unblockTopK(struct Queue* queue, unsigned int k, unsigned seq) {
// 	struct Node *curr = queue->head;
// 	unsigned int i = 0;
// 	if (curr == NULL)
// 		return;
// 	while(curr != NULL && i < k){
// 		curr->data.blocking = false;
//         curr->data.batch_seq = seq;
// 		curr = curr->next;
// 		i++;
// 	}
// 	return;
// }

// bool isHeadBlocked(struct Queue* queue){
// 	struct Node *curr = queue->head;
// 	if(curr==NULL)
// 		return true;
// 	return curr->data.blocking;
// }


// unsigned int bytesAhead(struct Queue* queue, unsigned int k) {
// 	struct Node *curr = queue->head;
// 	unsigned int i = 0;
// 	if (curr == NULL)
// 		return;
// 	while(curr != NULL && i < k){
// 		curr->data.blocking = false;
//         curr->data.batch_seq = seq;
// 		curr = curr->next;
// 		i++;
// 	}
// 	return;
// }


// int req_purge(struct Queue *queue, int sock, int object_id)
// {
// 	int index = 0, found =0;
// 	struct Node *curr = queue->head;
// 	struct Node *prev = curr;

// 	if (curr == NULL)
// 		return -1;

// 	while(curr != NULL){
// 		if((curr->data.sock == sock)&& 
// 		(curr->data.object_id == object_id)){
// 			if (curr == queue->head){
// 				if (queue->length == 1){
// 					queue->head = NULL;
// 					queue->tail = NULL;
// 				}
// 				else{
// 					queue->head = curr->next;
// 				}
// 			}
// 			else if(curr == queue->tail){
// 				prev->next = NULL;
// 				queue->tail = prev;

// 			}
// 			else{
// 				prev->next = curr->next;
// 			}
			
// 			curr->next = NULL;
// 			queue->length -=1;
// 			free(curr);
			
// 			return queue->length;	
// 		}

// 		prev = curr;
// 		curr = curr->next;
// 	}
	
// 	return -1;	

// }

int destroy_queue(struct Queue *queue)
{
	printf("Not implemented as of yet\n");
	return -1;
}

bool isEmpty(struct Queue *queue)
{
	if(queue->length==0){
		return true;
	}

	return false;
}

int len(struct Queue *queue){
	return queue->length;
}
