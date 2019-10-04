//
// Created by ferragut on 28/09/2019.
//

#ifndef EXP1_QUEUE_H

typedef struct QueueItemStruct{
    void* item;
    struct QueueItemStruct* next;
} QueueItem ;

typedef struct QueueStruct{
    int size;
    QueueItem* firstItem;
} Queue ;

Queue* create_queue();
void push(Queue* queue, void* new_item);
QueueItem* pop(Queue* queue);
int is_empty(Queue* queue);

/*   create_queue creates a new Queue
 * */
Queue* create_queue(){
    Queue* new_queue = malloc(sizeof(Queue));
    new_queue->size = 0;
    new_queue->firstItem = NULL;
    return new_queue;
}


/*   Push a value to the end of the Queue
 * */
void push(Queue* queue, void* new_item){
    QueueItem* current_node = queue->firstItem;
    QueueItem* new_node = malloc(sizeof(QueueItem));
    new_node->item = new_item;
    new_node->next = NULL;
    if (current_node != NULL){
        while(current_node->next != NULL){
            current_node = current_node->next;
        }
        current_node->next = new_node;
    }
    else{
        queue->firstItem = new_node;
    }
    queue->size++;

}

/*   Gets the first item of the queue and replaces it
 * */
QueueItem* pop(Queue* queue){
    if (is_empty(queue)){
        return NULL;
    }
    QueueItem* current_node = queue->firstItem;
    QueueItem* next_node = current_node->next;
    queue->firstItem = next_node;
    free(current_node);
    queue->size--;
    return current_node;

}

QueueItem* peek(Queue* queue){
    if (is_empty(queue)){
        return NULL;
    }
    QueueItem* current_node = queue->firstItem;
    return current_node;

}

int is_empty(Queue* queue){
    if (queue->size == 0)
        return 1;
    else
        return 0;
}

void print_queue(Queue* queue){
    QueueItem* current_node = queue->firstItem;

    if (is_empty(queue)){
        printf("Queue Empty!\n");
        return;
    }
    while(current_node != NULL){
        int * item_ptr = current_node->item;
        printf("Value:%d\n", *item_ptr);
        current_node = current_node->next;
    }
}

#define EXP1_QUEUE_H

#endif //EXP1_QUEUE_H
