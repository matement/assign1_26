#include "thread.h"
#include "queue.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define START_THREAD_COUNT 16
#define STACK_SIZE 16*1024 

static Queue_t *ready_queue;
static int global_thread_id = 0;
static int global_thread_count = 0;
extern void _swtch(void* from, void* to);
extern void _thrstart();

struct thread {
        int *sp;        //stack pointer
        int tid;        //thread id
        Queue_t *q;     //threads in queue when join is called
        int *startsp;
        void *argbuf;
        int exitcode;
        int joinret;
};

static struct lookup_table{
        Thread_t **table;
        int size;
} thread_lookup_table;

Thread_t *current;      //the thread that its running
static Thread_t *dead = NULL;      /* exited thread whose memory is not freed yet */
static Thread_t *joinall = NULL;   /* the thread blocked in Thread_join(0), if any */

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

static void remove_from_lookup_table(int id){
        assert(id > 0 && id < thread_lookup_table.size);
        thread_lookup_table.table[id] = NULL;
}

/* free the thread that exited earlier; safe only when we are NOT running on its stack */
static void reap(void) {
        if (dead != NULL && dead != current) {
                free(dead->startsp);
                free(dead->argbuf);
                free(dead->q);          /* the (now empty) joiner queue struct */
                free(dead);
                dead = NULL;
        }
}

/* current has already been put wherever it waits (a queue or joinall); run someone else */
static void block(void) {
        Thread_t *next = NULL;
        if (dequeue(&ready_queue, &next) == failure) {
                fprintf(stderr, "deadlock: every thread is blocked\n");
                exit(-1);
        }
        Thread_t *old = current;
        current = next;
        _swtch(old, next);
        reap();                          /* we resumed later; clean up whoever died meanwhile */
}

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
        assert(current != NULL);
        assert(func != NULL);
        assert(args != NULL || nbytes == 0);
        Thread_t *t = calloc(1, sizeof(Thread_t));
        if(t == NULL){
                fprintf(stderr, "Malloc failed in Thread_new\n");
                exit(-1);
        }
        t->startsp = malloc(STACK_SIZE);
        t->argbuf = malloc(nbytes ? nbytes : 1);
        if (t->startsp == NULL || t->argbuf == NULL) {
                fprintf(stderr, "malloc failed in Thread_new\n");
                exit(-1);
        }
        if(nbytes>0){
                memcpy(t->argbuf, args, nbytes);
        }

        uintptr_t top = ((uintptr_t)t->startsp + STACK_SIZE) & ~(uintptr_t)15;
        uintptr_t *frame = (uintptr_t *)(top - 32);

        frame[0] = 0;
        frame[1] = (uintptr_t)func;
        frame[2] = (uintptr_t)t->argbuf;
        frame[3] = 0;
        frame[4] = (uintptr_t) _thrstart;
        t->sp = (int *)frame;

        t->tid = ++global_thread_id;
        global_thread_count++;
        add_to_lookup_table(t);
        if(enqueue(&ready_queue, t)){
                fprintf(stderr, "enqueue failed in thread_new\n");
                exit(0);
        }
        return t->tid;
}
void Thread_exit(int code) {
        assert(current != NULL);
        reap();                          /* a previous dead thread, if we're on a fresh stack */
        Thread_t *self = current;
        self->exitcode = code;
        remove_from_lookup_table(self->tid);
        global_thread_count--;

        /* wake everyone joined on us, handing them our exit code */
        Thread_t *w = NULL;
        while (self->q != NULL && dequeue(&self->q, &w) == success) {
                w->joinret = code;
                if (enqueue(&ready_queue, w) == failure) { fprintf(stderr, "enqueue failed\n"); exit(-1); }
        }

        if (global_thread_count == 0)    /* last thread: the whole program ends */
                exit(code);

        /* only the join(0) waiter is left: wake it */
        if (joinall != NULL && global_thread_count == 1) {
                joinall->joinret = 0;
                if (enqueue(&ready_queue, joinall) == failure) { fprintf(stderr, "enqueue failed\n"); exit(-1); }
                joinall = NULL;
        }

        dead = self;                     /* freed later by someone else, never by us */
        Thread_t *next = NULL;
        if (dequeue(&ready_queue, &next) == failure) {
                fprintf(stderr, "deadlock: every thread is blocked\n");
                exit(-1);
        }
        current = next;
        _swtch(self, next); 
}
int Thread_self(void) { 
        return current->tid;
}
int Thread_join(int tid) {
                assert(current != NULL);
        if (tid == 0) {
                assert(joinall == NULL);                 /* checked error: only one join(0) */
                if (global_thread_count == 1) return 0;  /* we are the only thread */
                joinall = current;
                block();
                return 0;
        }
        assert(tid != current->tid);                     /* checked error: joining yourself */
        Thread_t *t = get_thread(tid);
        if (t == NULL) return -1;                        /* nonexistent (or already exited) */
        if (t->q == NULL) t->q = initq();
        if (enqueue(&t->q, current) == failure) { fprintf(stderr, "enqueue failed\n"); exit(-1); }
        block();
        return current->joinret;
}
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
