#include "thread.h"
#include "queue.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define START_THREAD_COUNT 16
#define STACK_SIZE 64*1024 

static Queue_t *ready_queue;
static int global_thread_id = 0;
static int global_thread_count = 0;
extern void _swtch(void* from, void* to);

struct thread {
        int *sp;        //stack pointer
        int tid;        //thread id
        Queue_t *q;     //threads in queue when join is called
        int *startsp;
        int exitcode;
};

static struct lookup_table{
        Thread_t **table;
        int size;
} thread_lookup_table;

Thread_t *current;      //the thread that its running

static void init_lookup_table(){
        thread_lookup_table.size = START_THREAD_COUNT;
        thread_lookup_table.table = malloc(START_THREAD_COUNT*sizeof(Thread_t*));
        if(thread_lookup_table.table == NULL){
                fprintf(stderr, "Initilaising the lookup table (table) failed\n");
                exit(-1);
        }
        for(int i = 0; i<START_THREAD_COUNT; i++){
                thread_lookup_table.table[i] = NULL;
        }
        return;
}

static void extend_lookup_table(){
        int new_size = thread_lookup_table.size*2;
        Thread_t **tmp = realloc(thread_lookup_table.table, new_size*sizeof(Thread_t*));
        if(tmp == NULL){
                fprintf(stderr, "Realloc failed\n");
                exit(-1);
        }
        thread_lookup_table.table = tmp;
        for(int i = thread_lookup_table.size; i< new_size; i++){
                thread_lookup_table.table[i] = NULL;
        }
        thread_lookup_table.size = new_size;
}

static void add_to_lookup_table(Thread_t *t){
        assert(t != NULL && t->tid>0);
        int id = t->tid;
        while(id>=thread_lookup_table.size){
                extend_lookup_table();
        }
        assert(thread_lookup_table.table[id] == NULL);
        thread_lookup_table.table[id] = t;
        return;
}

static Thread_t* get_thread(int id){
        if(id<=0 || id>=thread_lookup_table.size) return NULL;
        return thread_lookup_table.table[id];
}

/* dummy stubs so the project links; replace with the real implementation */
void Thread_init(void) {
        assert(current == NULL);
        Thread_t *t = malloc(sizeof(Thread_t));
        if(t == NULL){
                fprintf(stderr, "Malloc failed in Thread_init\n");
                exit(-1);
        }
        memset(t, 0, sizeof(Thread_t));
        t->sp = NULL;
        t->startsp = NULL;
        t->q = NULL;
        t->exitcode = 0;
        t->tid = ++global_thread_id;
        global_thread_count++;
        ready_queue = initq();
        init_lookup_table();
        add_to_lookup_table(t);
        current = t;
}
int Thread_new(int func(void *), void *args, size_t nbytes, ...) { 
        assert (current != NULL && func!= NULL && args != NULL && nbytes >= 0);
        Thread_t *t = calloc(1, sizeof(Thread_t));
        if(t == NULL){
                fprintf(stderr, "Malloc failed in Thread_new\n");
                exit(-1);
        }
        t->tid = ++global_thread_id;
        t->startsp = malloc(STACK_SIZE);
        add_to_lookup_table(t);
        return -1;
}
void Thread_exit(int code) { (void)code; }
int Thread_self(void) { 
        return current->tid;
}
int Thread_join(int tid) { (void)tid; return -1; }
void Thread_pause(void) {
        assert(current != NULL);

        if(isEmptyq(ready_queue)){
                printf("queue is empty\n");
                return;
        }
        else{
                enum retstate err;
                Thread_t *t;
                err = dequeue(&ready_queue, &t);
                if(err == failure){
                        printf("dequeue failed\n");
                        exit(-1);
                }
                Thread_t *temp = current;
                current = t;
                err = enqueue(&ready_queue, temp);
                if(err == failure){
                        printf("enqueue failed\n");
                        exit(-1);
                }
                _swtch(temp, t);
        }

}
