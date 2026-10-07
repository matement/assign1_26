#include "thread.h"
#include "queue.h"
#include <stdint.h>
#include <stdlib.h>

static Queue_t *ready_queue;
static int global_thread_id = 0;
static int global_thread_count = 0;


/* dummy stubs so the project links; replace with the real implementation */
Thread_t *Thread_init(void) {
        Thread_t *t = malloc(sizeof(Thread_t));
        t->tid = global_thread_id++;
        global_thread_count++;
        t->q = NULL;
        ready_queue = initq();
        return t;
}
int Thread_new(int func(void *), void *args, size_t nbytes, ...) { return -1; }
void Thread_exit(int code) { (void)code; }
int Thread_self(void) { return 0; }
int Thread_join(int tid) { (void)tid; return -1; }
void Thread_pause(void) {}
