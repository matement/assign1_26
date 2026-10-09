#include "thread.h"
#include "queue.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

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

Thread_t *current;      //the thread that its running

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
        current = t;
}
int Thread_new(int func(void *), void *args, size_t nbytes, ...) { return -1; }
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
