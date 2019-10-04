#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <map>

using namespace std;

// Max bats to signal of the same dir
#define MAX_TO_SIGNAL 4

#define N 1
#define S 2
#define E 3
#define W 4

// map to convert directions to integers
map<char, int> m;

// derefrencing the integer directions to strings to use it in output

char dir[5][10] = { "", "North", "East ", "South", "West " };

// bats in queue count
int queue_count[5] = { 0, 0, 0, 0, 0 };

// W vector for deadlock detection
bool waiting[5] = { 0, 0, 0, 0, 0 };

// just signaled threads of the same type to avoid starvation
int signaled[5] = { 0, 0, 0, 0, 0 };

// is ready
bool ready[5] = { 0, 0, 0, 0, 0 };

// Queue Condition variables , checks if turn for dir[i]
pthread_mutex_t queue_lock[5]; // Mohamed :  5 instead of 4 because indexing starts with 1
pthread_cond_t queue_cond[5];

// Turn Condition variables , checks if turn for dir[i]
pthread_mutex_t first_lock[5];
pthread_cond_t first_cond[5];

// locks the printer becuase it's not thread level atomic if we used multiple strings,
// this fixes an issue realted to the cin buffer.
pthread_mutex_t printer_lock;

// locks sleep for asynch calls
pthread_mutex_t sleep_lock;


//bat man each one has dirction and its number
struct bat {
    int NUM;
    int DIR; // Mohamed : changed it to an int to support the new DIR type
};
string input = ""; //user input
bat *batsArray;

bool running = true;

// Asynchronous print .. as the cout is a global variable that needs locking
#define  AS_PRINT(a)  pthread_mutex_lock(&printer_lock);   cout << a << endl;	pthread_mutex_unlock(&printer_lock)

// Asynch Sleep
#define  AS_SLEEP()  pthread_mutex_lock(&sleep_lock); sleep(1);  pthread_mutex_unlock(&sleep_lock);
// ===========================   Deadlock  Check =============================================

// Mohamed : asnych function to check for deadlock before waiting for the right

void* check(void *t) {

    while (running) {
        bool deadlock = true;
        for (int i = 1; i <= 4; i++)
            if (waiting[i] == false) {
                //AS_PRINT( "No Deadlock");
                deadlock = false;
                break;
            }

        if (deadlock) {

            // searching for max queued direction
            int max_i = 1;
            for(int i=1; i<=4; i++)
                if(queue_count[i] >= queue_count[max_i])
                    max_i = i;


            AS_PRINT( "DEADLOCK: BAT jam detected, signalling " << dir[max_i] << " to go");
            pthread_mutex_lock(&first_lock[max_i]);
            pthread_cond_signal(&first_cond[max_i]);
            pthread_mutex_unlock(&first_lock[max_i]);

        }
    }

}

// ===========================   BAT Methods { arrive, cross, leave }  =======================

// Nourhan : increments counter bta3 el bat elly da5el w lw fi wa7ed 2bloh b wait
// Mohamed : I used the direction trick to decrease the code
// Mohamed : 6:48 PM - Tested correctly
void arrive(void *t) {
    bat *b1 = (bat *) t;
    pthread_mutex_lock(&queue_lock[b1->DIR]);
    queue_count[b1->DIR]++; // Add the current bat to queue.
    while (queue_count[b1->DIR] > 1) { // while there are others in queue
        AS_PRINT( "Bat b" << b1->NUM << " from " << dir[b1->DIR] << " waits in queue");
        pthread_cond_wait(&queue_cond[b1->DIR], &queue_lock[b1->DIR]); // waits for a signal
    }

    AS_PRINT("Bat b" << b1->NUM << " from " << dir[b1->DIR] << " arrives crossing");
    // set direction as ready
    ready[b1->DIR] = true;
    pthread_mutex_unlock(&queue_lock[b1->DIR]);
}

void cross(bat* b1) {

    // circular mod .. to get the right bat
    int right_bat = ((b1->DIR - 1) + 1) % 4 + 1;

    // ============== Wait for turn =================

    // lock mutex first [ POSIX Threads note ]
    pthread_mutex_lock(&first_lock[b1->DIR]);
    //  wait for turn [ while checking for condition predicate ]
    while (queue_count[right_bat] > 0) {
        AS_PRINT( "Bat b" << b1->NUM << " from " << dir[b1->DIR] << " waits for crossing " << dir[right_bat]);
        // set the waiting vector for the deadlock checker
        waiting[b1->DIR] = true;
        // wait until turn is signaled
        pthread_cond_wait(&first_cond[b1->DIR], &first_lock[b1->DIR]);
    }

    // ============== Cross =================
    // asynch cross
    AS_PRINT( "Bat b" << b1->NUM << " from " << dir[b1->DIR] << " crossing");
    // asynch sleep
    AS_SLEEP();
    // reset waiting vector for deadlock checker
    waiting[b1->DIR] = false;

    // unlock mutex for future uses.
    pthread_mutex_unlock(&first_lock[b1->DIR]);
}

void leave(bat* b1) {
    // circular mod  .. to get the left bat
    int left_bat = ((b1->DIR - 1) + 3) % 4 + 1;

    // ============== Signal next in queue =================

    // lock queue mutex first [ POSIX Threads note ]
    pthread_mutex_lock(&queue_lock[b1->DIR]);
    // decrease counter
    queue_count[b1->DIR]--;
    ready[b1->DIR] = false;
    // signal waiting queue
    pthread_cond_signal(&queue_cond[b1->DIR]);
    // unlock mutex
    pthread_mutex_unlock(&queue_lock[b1->DIR]);

    // ============== Signal left starving =================

    // locking the count
    pthread_mutex_lock(&first_lock[b1->DIR]);

    // while threshold not encountered and there are still bats in queue
    if (signaled[b1->DIR] < MAX_TO_SIGNAL && queue_count[b1->DIR] > 0) {
        if (ready[b1->DIR]) {
            // increase counter
            signaled[b1->DIR]++;
            // signal one more thread in queue
            pthread_cond_signal(&first_cond[b1->DIR]);
        }

    } else {
        // starvation

        //  then reset counter
        signaled[b1->DIR] = 0;
        // lock mutex
        pthread_mutex_lock(&first_lock[left_bat]);
        // signal bat to the left starving
        pthread_cond_signal(&first_cond[left_bat]);
        // unlock mutex
        pthread_mutex_unlock(&first_lock[left_bat]);

    }
    pthread_mutex_unlock(&first_lock[b1->DIR]);
}

//===============================================================================
// ===========================       Bat complete thread  =======================
//===============================================================================

void *bat_thread(void *t) {
    bat* b = (bat*) t;
    arrive(b);
    cross(b);
    leave(b);
    pthread_exit(NULL);
}

// ===================================   Main procedure =============================================

// Nourhan : elly byd5ol b24of el direction elly 3la yminoh lw faduy ba3ady
//           bs lazem w ana tl3a a4of el count bta3y 34an mmkn 2kon 3amla starvation ll 3la 4maly
//           tyb lw kan mn el 2wal fi 7ad 3la ymeeny bwait
void createBats() { //fills bat array
    input = "ssss";
    batsArray = new bat[input.length()];
    int i;
    for (i = 0; i < input.length(); i++) {
        batsArray[i].NUM = i + 1;
        // Mohamed : setting the direction with its corresponding number
        // from the map, 'N' -> 1, 'E' -> 2 . etc
        batsArray[i].DIR = m[input.at(i)];
        cout << input.at(i) << " " << i + 1 << " " << m[input.at(i)] << endl;
    }
    cout << input.length() << endl;
}
int main(int argc, char *argv[]) { //de ba2a ll tagroba
    running = true;
    // Mohamed : initializing the map.
    m['N'] = m['n'] = 1;
    m['E'] = m['e'] = 2;
    m['S'] = m['s'] = 3;
    m['W'] = m['w'] = 4;

    int i, rc;
    createBats();
    pthread_t threads[input.length()];
    pthread_t batman;
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&printer_lock, NULL);
    pthread_mutex_init(&sleep_lock, NULL);


    for (i = 1; i <= 4; i++) {
        pthread_mutex_init(&first_lock[i], NULL);
        pthread_mutex_init(&queue_lock[i], NULL);
        pthread_cond_init(&queue_cond[i], NULL);
        pthread_cond_init(&first_cond[i], NULL);
    }

    pthread_create(&batman, NULL, check, &batsArray[0]);
    for (i = 0; i < input.length(); i++)
        pthread_create(&threads[i], &attr, bat_thread, &batsArray[i]);

    for (i = 0; i < input.length(); i++) {
        pthread_join(threads[i], NULL);
    }
    running = false;
    /* Clean up and exit */
    pthread_attr_destroy(&attr);


    for (i = 1; i <= 4; i++) {
        pthread_mutex_destroy(&first_lock[i]);
        pthread_mutex_destroy(&queue_lock[i]);
        pthread_cond_destroy(&queue_cond[i]);
        pthread_cond_destroy(&first_cond[i]);
    }
    pthread_mutex_destroy(&printer_lock);
    pthread_mutex_destroy(&sleep_lock);
    cout << "Done!" << endl;
    pthread_exit(NULL);

}