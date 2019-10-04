#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "bats.h"
#include "queue.h"

pthread_mutex_t mutex[5];
pthread_cond_t cond[4];
pthread_t threads[4];

Queue* queue_array[4];
int total_car_number = 0;
int car_id = 0;

int BIT_MASK[4] = {0,0,0,0};

void generate_queues();

void* BAT_behaviour(void* arg){
    Queue* queue_ptr = (Queue*)arg;
    BAT *car = (BAT*)peek(queue_ptr);

    printf("BAT %d %c chegou no cruzamento\n", car->car_number, toupper(car->direction));

    pthread_mutex_lock(&mutex);
    printf("[BAT %d %c] I AM DOING STUFF MKAY...\n", car->car_number, toupper(car->direction));
    sleep(1);
    printf("BAT %d %c saiu no cruzamento\n", car->car_number, toupper(car->direction));
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void generate_queues() {
    Queue* queueN = create_queue();
    queue_array[NORTH] = queueN;
    Queue* queueE = create_queue();
    queue_array[EAST] = queueE;
    Queue* queueS = create_queue();
    queue_array[SOUTH] = queueS;
    Queue* queueW = create_queue();
    queue_array[WEST] = queueW;
}

void generate_threads() {
    pthread_t north_thread;
    threads[NORTH] = north_thread;
    pthread_t east_thread;
    threads[EAST] = east_thread;
    pthread_t west_thread;
    threads[SOUTH] = west_thread;
    pthread_t south_thread;
    threads[WEST] = south_thread;
}

int get_current_direction(char cur_dir) {
    int index = 0;
    switch (cur_dir){
        case 'n':
            index = NORTH;
            break;
        case 'e':
            index = EAST;
            break;
        case 's':
            index = SOUTH;
            break;
        case 'w':
            index = WEST;
            break;
    }

    return index;
}

void print_bitmask() {
    for(int i = 0; i<4; i++) {
        printf("END_BIT_MASK[%d]:%d\n", i, BIT_MASK[i]);
    }
}

void print_queue_array() {
    for(int i = 0; i<4; i++) {
        print_queue(queue_array[i]);
    }
}

void* generate_BIT_MASK(char* directions_string) {
    printf("GENERATE_BIT_MASK\n");

    for(int i=0; i<strlen(directions_string)-1; i++){
        char cur_dir = directions_string[i];
        int current_direction_index = get_current_direction(cur_dir);
        BIT_MASK[current_direction_index]++;
    }
}

int check_priority() {
    return NORTH;
}


void BAT_handler(void *arg) {
    // cast de void para BAT

    // lock
//    BIT_MASK = generate_BIT_MASK();
    // unlock
}

void generate_BATS() {
    for(int bitmask_index = 0; bitmask_index<4; bitmask_index++){
        for(int bat_index = 0; bat_index < BIT_MASK[bitmask_index]; bat_index++) {
            car_id++;
            BAT* new_car_BAT = new_BAT(bitmask_index, car_id);
            push(queue_array[bitmask_index], new_car_BAT);
        }
    }
}

void* BAT_manager(void *arg){
//  TODO:Criar threads de filas (4)
//  TODO:Para cada char da string, criar novo carro

    generate_queues();
    generate_threads();

    char* directions_string = (char*) arg;

    generate_BIT_MASK(directions_string);
    generate_BATS();

    int priority = check_priority();

    pthread_t* thread_to_go = (pthread_t *) threads[priority];
    Queue* queue_to_go = queue_array[priority];

    pthread_create(thread_to_go, NULL, BAT_behaviour, queue_to_go);

//    char current_direction = directions_string[0];
//    int car_number = 0;
//    while (current_direction != '\n'){
//        pthread_t* new_car_thread = malloc(sizeof(pthread_t));
//        car_number++;
//        total_car_number++;
//        pthread_mutex_lock(&mutex);
//        BAT* new_car_BAT = new_BAT(current_direction, total_car_number);
//        pthread_mutex_unlock(&mutex);
//        pthread_create(new_car_thread, NULL, BAT_behaviour, new_car_BAT);
//        current_direction = directions_string[car_number];
//    }

//    printf("[BATMAN]I am DONE!...\n");
    return NULL;

}


int main() {
    printf("Starting program!\n");
    char    *buffer;
    size_t  n = 1024;
    buffer = malloc(n);

    // While an empty line is not read, continue reading
    while(getline(&buffer, &n, stdin) != 1){
        BAT_manager(buffer);
    }

    printf("Bye Bye!\n");
    return 0;
}